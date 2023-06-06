#ifndef VECTOR_H
#define VECTOR_H

#define DEBUG 0

typedef struct vector_t vector_t;

typedef enum vector_ret_t
{
	VECTOR_SUCCESS = 0,
	VECTOR_FAILURE = 1,
	VECTOR_OVERFLOW,
	VECTOR_UNDERFLOW,
	VECTOR_EMPTY,
	VECTOR_FULL,
	VECTOR_NONEMPTY
} vector_ret_t;

/**
 * Creates a circular vector with `capacity` elements at most.
 *
 * RETURN VALUES:
 * vector_t pointer
 * NULL pointer -- when failed to allocate memory
 * 
 * [in] - capacity
 */
vector_t* vector_create(size_t capacity);

/**
 * Destroys the entire vector with all its elements.
 *
 * RETURN VALUES:
 * VECTOR_SUCCESS -- vector is destroyed
 * VECTOR_FAILURE -- vector is invalid
 * 
 * [in] - vector
 */
vector_ret_t vector_destroy(vector_t* vector);

/**
 * Adds an element to the vector.
 *
 * RETURN VALUES:
 * VECTOR_SUCCESS
 * VECTOR_OVERFLOW -- vector is full and the first element is overwritten
 * VECTOR_FAILURE -- vector or element is NULL pointer
 * 
 * [in] - vector, element
 */
vector_ret_t vector_push_begin(vector_t* vector, void* element);

/**
 * Adds an element to the vector.
 *
 * RETURN VALUES:
 * VECTOR_SUCCESS
 * VECTOR_OVERFLOW -- vector is full and the last element is overwritten
 * VECTOR_FAILURE -- vector or element is NULL pointer
 *
 * [in] - vector, element
 */
vector_ret_t vector_push_end(vector_t* vector, void* element);

/**
 * Removes an element from the vector.
 *
 * RETURN VALUES:
 * VECTOR_SUCCESS
 * VECTOR_UNDERFLOW -- vector is empty
 * VECTOR_FAILURE -- vector or p_element are NULL pointers
 * 
 * [in] - vector
 * [out] - p_element
 */
vector_ret_t vector_pop_begin(vector_t* vector, void** p_element);

/**
 * Removes an element from the vector.
 * RETURN VALUES:
 * VECTOR_SUCCESS
 * VECTOR_UNDERFLOW -- vector is empty
 * VECTOR_FAILURE -- vector or p_element are NULL pointers
 * 
 * [in] - vector
 * [out] - p_element
 */
vector_ret_t vector_pop_end(vector_t* vector, void** p_element);

/**
 * Peeks the first element at the front of the vector.
 */
vector_ret_t vector_peek_begin(const vector_t* vector, void** p_element);

/**
 * Peeks the last element at the end of the vector.
 *
 * RETURN VALUES:
 * VECTOR_SUCCESS
 * VECTOR_UNDERFLOW -- vector is empty
 * VECTOR_FAILURE -- vector or p_element are NULL pointers
 * 
 * [in] - vector
 * [out] - p_element
 */
vector_ret_t vector_peek_end(const vector_t* vector, void** p_element);

/**
 * RETURN VALUES:
 * VECTOR_FULL
 * VECTOR_EMPTY
 * VECTOR_NONEMPTY
 * VECTOR_FAILURE -- vector is NULL pointer
 * 
 * [in] - vector
 */
vector_ret_t vector_status(const vector_t* vector);

/*
* RETURN VALUES:
* VECTOR_SUCCESS
* VECTOR_FAILURE -- vector or capacity is NULL pointer
* 
* [in] - vector
* [out] - capacity
*/
vector_ret_t vector_get_capacity(const vector_t* vector, size_t* capacity);

#endif // VECTOR_H

