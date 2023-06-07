#include <stdlib.h>
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

	pthread_mutex_t mutex;
	pthread_cond_t cond;
};



/*
* FUNCTION DECLARATIONS
*/



vector_t* vector_create(const size_t capacity);
vector_ret_t vector_destroy(vector_t* vector);

vector_ret_t vector_push(vector_t* vector, void* element);

vector_ret_t vector_pop(vector_t* vector, void** element);

static vector_ret_t vector_enlarge(vector_t* vector);

static inline size_t vector_next_index(const size_t index, const size_t capacity);
static inline size_t vector_prev_index(const size_t index, const size_t capacity);



/*
* FUNCTION DEFINITIONS
*/



vector_t* vector_create(const size_t capacity)
{
	vector_t* vector = malloc(sizeof(*vector));

	if (vector == NULL)
	{
		debug_print("Not enough memory for capacity: %zd\n", capacity);
		return NULL;
	}

	vector->element = malloc((capacity + 1) * sizeof(vector->element[0])); // we add one more cell because end index is exclusive

	if (vector->element == NULL)
	{
		debug_print("Not enough memory for capacity: %zd\n", capacity);
		free(vector);
		return NULL;
	}

	vector->capacity = capacity;
	vector->begin = vector->end = 0;

	if (pthread_mutex_init(&vector->mutex, NULL) != 0 ||
		pthread_cond_init(&vector->cond, NULL) != 0) {
		vector_destroy(vector);
		return NULL;
	}

	return vector;
}

vector_ret_t vector_destroy(vector_t* vector)
{
	CHECK_AND_RETURN_IF_NOT_EXIST(vector);

	pthread_mutex_destroy(&vector->mutex);
	pthread_cond_destroy(&vector->cond);

	free(vector->element);
	free(vector);
	
	return VECTOR_SUCCESS;
}

vector_ret_t vector_push(vector_t* vector, void* element)
{
	CHECK_AND_RETURN_IF_NOT_EXIST(vector);

	if (pthread_mutex_lock(&vector->mutex) != 0)
		return VECTOR_FAILURE;

	if (vector_next_index(vector->end, vector->capacity) == vector->begin) {	// Vector full condition
		if (vector_enlarge(vector) != VECTOR_SUCCESS) {
			return VECTOR_FAILURE;
		}
	}

	vector->element[vector->end] = element;
	vector->end = vector_next_index(vector->end, vector->capacity);

	if (pthread_cond_signal(&vector->cond) != 0) {
		pthread_mutex_unlock(&vector->mutex);
		return VECTOR_FAILURE;
	}

	if (pthread_mutex_unlock(&vector->mutex) != 0)
		return VECTOR_FAILURE;

	return VECTOR_SUCCESS;
}

vector_ret_t vector_pop(vector_t* vector, void** p_element)
{
	CHECK_AND_RETURN_IF_NOT_EXIST(vector);
	CHECK_AND_RETURN_IF_NOT_EXIST(p_element);

	if (pthread_mutex_lock(&vector->mutex) != 0)
		return VECTOR_FAILURE;

	while (vector->begin == vector->end)  // Vector empty condition
	{
		if (pthread_cond_wait(&vector->cond, &vector->mutex) != 0) {
			pthread_mutex_unlock(&vector->mutex);
			return VECTOR_FAILURE;
		}
	}

	*p_element = vector->element[vector->begin];
	vector->begin = vector_next_index(vector->begin, vector->capacity);

	if (pthread_mutex_unlock(&vector->mutex) != 0)
		return VECTOR_FAILURE;

	return VECTOR_SUCCESS;
}

static vector_ret_t vector_enlarge(vector_t* vector) {
	void* new_location = NULL;

	if (vector->capacity == 0)
		new_location = malloc(2 * sizeof(vector->element[0])); // minimum space for one efficient cell
	else
		new_location = malloc((vector->capacity * 2 + 1) * sizeof(vector->element[0]));

	if (new_location == NULL)
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

	vector->capacity = vector->capacity * 2 + 1;

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