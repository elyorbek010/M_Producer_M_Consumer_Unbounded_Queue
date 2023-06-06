#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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
	size_t begin;
	size_t end;
	void** element;
};



/*
* FUNCTION DECLARATIONS
*/



vector_t* vector_create(const size_t capacity);
vector_ret_t vector_destroy(vector_t* vector);

vector_ret_t vector_push_end(vector_t* vector, void* element);

vector_ret_t vector_pop_begin(vector_t* vector, void* element);

vector_ret_t vector_peek_begin(const vector_t* vector, void** p_element);
vector_ret_t vector_peek_end(const vector_t* vector, void** p_element);

vector_ret_t vector_status(const vector_t* vector);

vector_ret_t vector_get_capacity(const vector_t* vector, size_t* capacity);

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

	vector->element = malloc((capacity + 1) * sizeof(vector->element[0]));
	
	if (vector->element == NULL)
	{
		debug_print("Not enough memory for capacity: %zd\n", capacity);
		free(vector);
		return NULL;
	}

	vector->capacity = capacity;
	vector->begin = vector->end = 0;

	return vector;
}

vector_ret_t vector_destroy(vector_t* vector)
{
	CHECK_AND_RETURN_IF_NOT_EXIST(vector);
	free(vector->element);
	free(vector);
	
	return VECTOR_SUCCESS;
}

vector_ret_t vector_push_end(vector_t* vector, void* element)
{
	CHECK_AND_RETURN_IF_NOT_EXIST(vector);

	vector->element[vector->end] = element;
	vector->end = vector_next_index(vector->end, vector->capacity);

	if (vector->begin == vector->end)
	{
		vector->begin = vector_next_index(vector->begin, vector->capacity);
		return VECTOR_OVERFLOW;
	}
	return VECTOR_SUCCESS;
}

vector_ret_t vector_pop_begin(vector_t* vector, void** p_element)
{
	CHECK_AND_RETURN_IF_NOT_EXIST(vector);
	CHECK_AND_RETURN_IF_NOT_EXIST(p_element);

	if (vector->begin == vector->end)
	{
		return VECTOR_UNDERFLOW;
	}

	*p_element = vector->element[vector->begin];
	vector->begin = vector_next_index(vector->begin, vector->capacity);
	return VECTOR_SUCCESS;
}

vector_ret_t vector_peek_begin(const vector_t* vector, void** p_element)
{
	CHECK_AND_RETURN_IF_NOT_EXIST(vector);
	CHECK_AND_RETURN_IF_NOT_EXIST(p_element);

	if (vector->begin == vector->end)
	{
		return VECTOR_UNDERFLOW;
	}

	*p_element = vector->element[vector->begin];
	return VECTOR_SUCCESS;
}

vector_ret_t vector_peek_end(const vector_t* vector, void** p_element)
{
	CHECK_AND_RETURN_IF_NOT_EXIST(vector);
	CHECK_AND_RETURN_IF_NOT_EXIST(p_element);

	if (vector->begin == vector->end)
	{
		return VECTOR_UNDERFLOW;
	}

	*p_element = vector->element[vector_prev_index(vector->end, vector->capacity)];
	return VECTOR_SUCCESS;
}

vector_ret_t vector_status(const vector_t* vector)
{
	CHECK_AND_RETURN_IF_NOT_EXIST(vector);

	if (vector_next_index(vector->end, vector->capacity) == vector->begin)
	{
		return VECTOR_FULL;
	}
	else if (vector->begin == vector->end)
	{
		return VECTOR_EMPTY;
	}
	else
	{
		return VECTOR_NONEMPTY;
	}
}

vector_ret_t vector_get_capacity(const vector_t* vector, size_t* capacity) {
	CHECK_AND_RETURN_IF_NOT_EXIST(vector);
	CHECK_AND_RETURN_IF_NOT_EXIST(capacity);

	*capacity = vector->capacity;
	return VECTOR_SUCCESS;
}

static inline size_t vector_next_index(const size_t index, const size_t capacity)
{
	return (index + 1) % (capacity + 1);
}

static inline size_t vector_prev_index(const size_t index, const size_t capacity)
{
	return index == 0 ? capacity : index - 1;
}