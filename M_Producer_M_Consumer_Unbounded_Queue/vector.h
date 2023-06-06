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
 */
vector_t* vector_create(size_t capacity);

/**
 * Destroys the entire cvector with all its elements.
 */
vector_ret_t vector_destroy(vector_t* cvector);

/**
 * Adds an element to the cvector.
 *
 * If the vector is full, VECTOR_OVERFLOW is returned
 * and `element` is written in place of the first element.
 */
vector_ret_t vector_push_end(vector_t* cvector, void* element);

/**
 * Adds an element to the cvector.
 *
 * If the vector is full, VECTOR_OVERFLOW is returned
 * and `element` is written in place of the last element.
 */
vector_ret_t vector_push_begin(vector_t* cvector, void* element);

/**
 * Removes an element from the vector.
 */
vector_ret_t vector_pop_begin(vector_t* cvector, void** p_element);

/**
 * Removes an element from the vector.
 */
vector_ret_t vector_pop_end(vector_t* cvector, void** p_element);

/**
 * Peeks the first element at the front of the vector.
 */
vector_ret_t vector_peek_begin(const vector_t* cvector, void** p_element);

/**
 * Peeks the last element at the end of the vector.
 */
vector_ret_t vector_peek_end(const vector_t* cvector, void** p_element);

/**
 * Returns VECTOR_FULL
 * Returns VECTOR_EMPTY
 * Returns VECTOR_NONEMPTY
 */
vector_ret_t vector_status(const vector_t* cvector);

/*
* Returns VECTOR_FAILURE if vector does not exists
* Returns VECTOR_SUCCESS otherwise
*/
vector_ret_t vector_get_capacity(const vector_t* cvector, size_t* capacity);

#endif // VECTOR_H

