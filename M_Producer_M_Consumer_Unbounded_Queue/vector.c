/*
* Thread-Safe Unbounded Vector.
* 
* Vector's working principles:
* Vector of size 3
*   |1|2|3|.|
*    |     |
* [begin  end)
* 
* Begin index is inclusive, the element at that index exists
* End index is exclusive, the element at that index does not exist
* 
* Corner cases:
*	'begin == end' -- vector is empty, because 'end' is exclusive
*	'next(end) == begin' -- vector is full, because 'end' catched up 'begin'
* 
* Vector growth by factor of 2 every time it overflows.
* 
* Vector mutex is locked before modifying vector data
* e.g. when pushing, popping, and expanding capacity
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "vector.h"
#include "debug.h"

#define CHECK_AND_RETURN_IF_NOT_EXIST(pointer_object)  \
    do{                                                \
        if (pointer_object == NULL)                    \
        {                                              \
            debug_print("Object does not exist\n");    \
            return VECTOR_FAILURE;                     \
        }                                              \
    }while(0)

struct vector_t
{
	size_t capacity;
	size_t begin;		// begin index is inclusive
	size_t end;			// end index is exclusive

	void** element;

	pthread_mutex_t vector_guard;
	pthread_cond_t avail;
};

/*
* FUNCTION DECLARATIONS
*/

vector_t* vector_create(const size_t capacity);
vector_ret_t vector_destroy(vector_t* vector);

vector_ret_t vector_push(vector_t* vector, void* element);
static vector_ret_t vector_push_impl(vector_t* vector, void* element);

vector_ret_t vector_pop(vector_t* vector, void** element);
static vector_ret_t vector_pop_impl(vector_t* vector, void** element);

static vector_ret_t vector_expand(vector_t* vector);

static inline size_t vector_next_index(const size_t index, const size_t capacity);
static inline size_t vector_prev_index(const size_t index, const size_t capacity);

/*
* FUNCTION DEFINITIONS
*/

vector_t* vector_create(size_t capacity)
{
	vector_t* vector = malloc(sizeof(*vector));

	if (vector == NULL)		// condition that malloc() failed
	{
		debug_print("Not enough memory for capacity: %zu\n", capacity);
		return NULL;
	}

	// Allocate one more cell because end index is exclusive
	vector->element = malloc((capacity + 1) * sizeof(vector->element[0])); 

	if (vector->element == NULL)	// condition that malloc() failed
	{
		debug_print("Not enough memory for capacity: %zu\n", capacity);
		free(vector);
		return NULL;
	}

	vector->capacity = capacity;
	vector->begin = vector->end = 0;

	if (pthread_mutex_init(&vector->vector_guard, NULL) != 0 ||
		pthread_cond_init(&vector->avail, NULL) != 0) {
		debug_print("Could not initialize vector_guard or conditional variable\n");
		vector_destroy(vector);
		return NULL;
	}

	debug_print("Vector elements address: %p with capacity: %zu\n", 
				vector->element, vector->capacity);

	return vector;
}

vector_ret_t vector_destroy(vector_t* vector)
{
	CHECK_AND_RETURN_IF_NOT_EXIST(vector);

	pthread_mutex_destroy(&vector->vector_guard);
	pthread_cond_destroy(&vector->avail);

	free(vector->element);
	free(vector);

	return VECTOR_SUCCESS;
}

vector_ret_t vector_push(vector_t* vector, void* element)
{
	CHECK_AND_RETURN_IF_NOT_EXIST(vector);

	if (pthread_mutex_lock(&vector->vector_guard) != 0)
		return VECTOR_FAILURE;

	if (vector_push_impl(vector, element) != VECTOR_SUCCESS) {
		pthread_mutex_unlock(&vector->vector_guard);
		return VECTOR_FAILURE;
	}

	debug_print("Push: %p at index: %zu\n", 
		element, 
		vector_prev_index(vector->end, vector->capacity));

	if (pthread_mutex_unlock(&vector->vector_guard) != 0)
		return VECTOR_FAILURE;

	return VECTOR_SUCCESS;
}

static vector_ret_t vector_push_impl(vector_t* vector, void* element) {
	// Expand vector first if FULL
	if (vector_next_index(vector->end, vector->capacity) == vector->begin) {
		if (vector_expand(vector) != VECTOR_SUCCESS) {
			debug_print("Could not expand vector\n");
			return VECTOR_FAILURE;
		}
	}

	vector->element[vector->end] = element;								
	vector->end = vector_next_index(vector->end, vector->capacity);		

	if (pthread_cond_signal(&vector->avail) != 0)
		return VECTOR_FAILURE;

	return VECTOR_SUCCESS;
}

vector_ret_t vector_pop(vector_t* vector, void** p_element)
{
	CHECK_AND_RETURN_IF_NOT_EXIST(vector);
	CHECK_AND_RETURN_IF_NOT_EXIST(p_element);

	if (pthread_mutex_lock(&vector->vector_guard) != 0)
		return VECTOR_FAILURE;

	if (vector_pop_impl(vector, p_element) != 0) {
		pthread_mutex_unlock(&vector->vector_guard);
		return VECTOR_FAILURE;
	}
	
	debug_print("Pop: %p at index: %zu\n",
		vector->element[vector_prev_index(vector->begin, vector->capacity)],
		vector_prev_index(vector->begin, vector->capacity));

	if (pthread_mutex_unlock(&vector->vector_guard) != 0)
		return VECTOR_FAILURE;

	return VECTOR_SUCCESS;
}

static vector_ret_t vector_pop_impl(vector_t* vector, void** p_element) {
	while (vector->begin == vector->end)  // Vector is EMPTY
	{
		if (pthread_cond_wait(&vector->avail, &vector->vector_guard) != 0)
			return VECTOR_FAILURE;
	}

	*p_element = vector->element[vector->begin];
	vector->begin = vector_next_index(vector->begin, vector->capacity);

	return VECTOR_SUCCESS;
}

static vector_ret_t vector_expand(vector_t* vector) {
	size_t front_idx = vector->begin;
	size_t end_idx = vector->end;

	size_t old_capacity = vector->capacity;

	size_t new_capacity = (old_capacity == 0) ? 1 : 2 * old_capacity;

	size_t old_actual_capacity = old_capacity + 1;	// one more cell because 'end' is exlusive
	size_t new_actual_capacity = new_capacity + 1;	// --||--

	size_t cell_size = sizeof(vector->element[0]);

	void** old_location = vector->element;

	void** new_location = malloc(new_actual_capacity * cell_size);	

	if (new_location == NULL)
		return VECTOR_FAILURE;

	/*since the vector is cyclical, we must ensure that data does not partition incorrectly
	* e.g. vector of size 3
	*	   '.' means empty
	*
	* e.g. |1|2|3|.| when enlarged becomes |1|2|3|.|.|.|.|.|
	*       |     |                         |     |
	*   front[0]  |				       front[0]   |
	*           end(3)						    end(3)
	*
	* Howerver,
	*
	*      |3|.|1|2| when enlarged might become  |3|.|1|2|.|.|.|.|
	*         | |                                   | |
	*         | front[2]					        | front[2] -- still overflow condition, 
	*         end(1)						        end(1)        next(end) == front
	*
	* so, we need to copy it linearly and reset 'begin' and 'end' indexes
	*/

	if (front_idx <= end_idx) {
		memcpy(new_location, old_location, old_actual_capacity * cell_size);
	}
	else {
		// Copy from 'front_idx' till end
		memcpy(new_location, 
			old_location + front_idx, 
			(old_actual_capacity - front_idx) * cell_size);	
		
		// Copy from beginning till 'end_idx'
		memcpy(new_location,
			old_location,
			end_idx * cell_size);

		front_idx = 0;										
		end_idx = (old_actual_capacity - front_idx) + end_idx; // vector length
	}

	free(old_location);

	// update vector object
	vector->capacity = new_capacity;
	vector->begin = front_idx;
	vector->end = end_idx;
	vector->element = new_location;

	debug_print("New vector elements address: %p with capacity: %zu\n", 
				vector->element, vector->capacity);

	return VECTOR_SUCCESS;
}

static inline size_t vector_next_index(const size_t index, const size_t capacity)
{
	// note: actual allocated capacity is 'capacity + 1'
	return (index + 1) % (capacity + 1);	
}

static inline size_t vector_prev_index(const size_t index, const size_t capacity)
{
	// note: actual allocated capacity is 'capacity + 1'
	return index == 0 ? capacity : index - 1; 
}