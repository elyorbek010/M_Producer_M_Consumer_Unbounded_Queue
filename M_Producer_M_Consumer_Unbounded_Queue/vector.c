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

static vector_ret_t vector_enlarge(vector_t* vector);

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

	vector->element = malloc((capacity + 1) * sizeof(vector->element[0])); // we add one more cell because end index is exclusive

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

	debug_print("Vector elements address: %p with capacity: %zu\n", vector->element, vector->capacity);

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

	if (pthread_mutex_unlock(&vector->vector_guard) != 0)
		return VECTOR_FAILURE;

	return VECTOR_SUCCESS;
}

static vector_ret_t vector_push_impl(vector_t* vector, void* element) {
	if (vector_next_index(vector->end, vector->capacity) == vector->begin) {	// Vector full condition
		if (vector_enlarge(vector) != VECTOR_SUCCESS) {	
			debug_print("Could not enlarge vector\n");
			return VECTOR_FAILURE;
		}
	}

	vector->element[vector->end] = element;								// insert the element
	vector->end = vector_next_index(vector->end, vector->capacity);		// update the 'end' index

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

	vector_pop_impl(vector, p_element);

	if (pthread_mutex_unlock(&vector->vector_guard) != 0)
		return VECTOR_FAILURE;

	return VECTOR_SUCCESS;
}

static vector_ret_t vector_pop_impl(vector_t* vector, void** p_element) {
	while (vector->begin == vector->end)  // Vector empty condition
	{
		if (pthread_cond_wait(&vector->avail, &vector->vector_guard) != 0)
			return VECTOR_FAILURE;
	}

	*p_element = vector->element[vector->begin];
	vector->begin = vector_next_index(vector->begin, vector->capacity);

	return VECTOR_SUCCESS;
}

static vector_ret_t vector_enlarge(vector_t* vector) {
	void* new_location = NULL;

	if (vector->capacity == 0) {
		new_location = malloc(2 * sizeof(vector->element[0])); // minimum space for one efficient cell
	} else {
		new_location = malloc((vector->capacity * 2 + 1) * sizeof(vector->element[0])); // double capacity
	}

	if (new_location == NULL)	// condition that malloc() failed
		return VECTOR_FAILURE;

	/*since the vector is cyclical, we must ensure that data does not partition incorrectly
	* e.g. vector of size 3
	* 
	* e.g. |1|2|3|.| when enlarged becomes |1|2|3|.|.|.|.|.|
	*       |     |                         |     |
	*   begin(1)  |				       begin(1)   |
	*           end(.)						    end(.)
	* 
	* Howerver, 
	* 
	*      |3|.|1|2| when enlarged might become  |3|.|1|2|.|.|.|.|
	*         | |                                   | |
	*         | begin(1)					        | begin(1) -- still overflow condition
	*         end(.)						        end(.)
	* 
	* so, we need to copy it linearly and reset 'begin' and 'end' indexes
	*/

	if (vector->begin < vector->end) 
	{
		memcpy(new_location, vector->element, (vector->capacity + 1) * sizeof(vector->element[0]));
	} 
	else 
	{
		memcpy(new_location, vector->element + vector->begin, (vector->capacity + 1 - vector->begin) * sizeof(vector->element[0]));
		memcpy(new_location, vector->element, vector->end * sizeof(vector->element[0]));

		vector->begin = 0;
		vector->end = (vector->capacity + 1 - vector->begin) + vector->end;
	}

	vector->capacity = vector->capacity * 2;

	free(vector->element);
	vector->element = new_location;

	debug_print("New vector elements address: %p with capacity: %zu\n", vector->element, vector->capacity);

	return VECTOR_SUCCESS;
}

static inline size_t vector_next_index(const size_t index, const size_t capacity)
{
	return (index + 1) % (capacity + 1);	// increment index, maximum number of cells = capacity + 1 since end index is exclusive
}

static inline size_t vector_prev_index(const size_t index, const size_t capacity)
{
	return index == 0 ? capacity : index - 1;
}