extern "C" {
#include "../vector.h"
}

#include "gtest/gtest.h"
#include <thread>
#include <vector>
#include <algorithm>
#include <numeric>
#include <unistd.h>

typedef struct
{
	size_t vector_size;
	size_t data_amount;
	size_t producers_n;
	size_t consumers_n;
	size_t producer_sleep;
	size_t consumer_sleep;
} mpmc_sim_opt_t;

void mpmc_simulate(mpmc_sim_opt_t options);

/* Call functions with invalid(NULL) pointers*/
TEST(BASIC_OP, NULL_INPUT_TEST) {
	vector_t* vector;

	vector = vector_create(5);

	int val = 0;
	void* data_ptr = NULL;

	EXPECT_EQ(vector_push(vector, nullptr), VECTOR_SUCCESS); // NULL is a valid value

	EXPECT_EQ(vector_push(nullptr, &val), VECTOR_FAILURE);
	EXPECT_EQ(vector_push(nullptr, nullptr), VECTOR_FAILURE);

	EXPECT_EQ(vector_pop(vector, nullptr), VECTOR_FAILURE);
	EXPECT_EQ(vector_pop(nullptr, &data_ptr), VECTOR_FAILURE);
	EXPECT_EQ(vector_pop(nullptr, nullptr), VECTOR_FAILURE);

	EXPECT_EQ(vector_destroy(nullptr), VECTOR_FAILURE);

	vector_destroy(vector);
}

TEST(BASIC_OP, Push_Pop)
{
	vector_t* vector = nullptr;

	int val = 0;
	void* data_ptr = NULL;

	vector = vector_create(5);

	EXPECT_EQ(vector_push(vector, &val), VECTOR_SUCCESS);

	EXPECT_EQ(vector_pop(vector, &data_ptr), VECTOR_SUCCESS);

	EXPECT_EQ(data_ptr, &val);

	vector_destroy(vector);
}

/*
* When queue overflows, it must allocate more space to fit in the new data
*/
TEST(BASIC_OP, ZeroCapacity_Overflow)
{
	const size_t num_of_data = 10;

	vector_t* vector = nullptr;
	void* data_ptr = nullptr;

	vector = vector_create(0);

	for (size_t data_n = 0; data_n < num_of_data; data_n++) {
		EXPECT_EQ(vector_push(vector, (void*)data_n), VECTOR_SUCCESS);
	}

	for (size_t data_n = 0; data_n < num_of_data; data_n++) {
		EXPECT_EQ(vector_pop(vector, &data_ptr), VECTOR_SUCCESS);
		EXPECT_EQ((size_t)data_ptr, data_n);
	}

	vector_destroy(vector);
}

TEST(BASIC_OP, FullVector_Overflow)
{
	const size_t num_of_data = 10;

	vector_t* vector = nullptr;
	void* data_ptr = nullptr;

	vector = vector_create(4);

	for (size_t data_n = 0; data_n < num_of_data; data_n++) {
		EXPECT_EQ(vector_push(vector, (void*)data_n), VECTOR_SUCCESS);
	}

	for (size_t data_n = 0; data_n < num_of_data; data_n++) {
		EXPECT_EQ(vector_pop(vector, &data_ptr), VECTOR_SUCCESS);
		EXPECT_EQ((size_t)data_ptr, data_n);
	}

	vector_destroy(vector);
}

/*
* Check that values can be inserted without blocking or waiting if enough space exists
*/
TEST(BASIC_OP, Circulation)
{
	vector_t* vector = nullptr;
	void* data_ptr;

	vector = vector_create(5);

	// push, vector elem 5/5
	for (size_t i = 0; i < 5; i++) {
		ASSERT_EQ(vector_push(vector, (void*)i), VECTOR_SUCCESS);
	}

	// pop, vector elem 0/5
	for (size_t i = 0; i < 5; i++) {
		ASSERT_EQ(vector_pop(vector, &data_ptr), VECTOR_SUCCESS);
		ASSERT_EQ((size_t)data_ptr, i);
	}

	// push, vector elem 5/5
	for (size_t i = 6; i < 10; i++) {
		ASSERT_EQ(vector_push(vector, (void*)i), VECTOR_SUCCESS);
	}

	// pop, vector elem 0/5
	for (size_t i = 6; i < 10; i++) {
		ASSERT_EQ(vector_pop(vector, &data_ptr), VECTOR_SUCCESS);
		ASSERT_EQ((size_t)data_ptr, i);
	}

	vector_destroy(vector);
}

TEST(BASIC_OP, Circulation_Overflow)
{
	vector_t* vector = nullptr;

	vector = vector_create(5); // begin - 0, end - 0

	void* data_ptr = nullptr;

	// push, vector elem, elements 5/5 
	for (size_t i = 0; i < 5; i++) {
		ASSERT_EQ(vector_push(vector, (void*)i), VECTOR_SUCCESS);
	}																// begin - 0, end - 5

	// pop, vector elem, elements 2/5
	for (size_t i = 0; i < 3; i++) {
		ASSERT_EQ(vector_pop(vector, &data_ptr), VECTOR_SUCCESS);
		ASSERT_EQ((size_t)data_ptr, i);
	}																// begin - 3, end - 5

	// push, vector elem, elements 6/5 -> 6/10
	for (size_t i = 5; i < 9; i++) {
		ASSERT_EQ(vector_push(vector, (void*)i), VECTOR_SUCCESS);	// begin - 3, end - 2 -> overflow
	}																// begin - 0, end - 6

	for (size_t i = 3; i < 9; i++) {
		ASSERT_EQ(vector_pop(vector, &data_ptr), VECTOR_SUCCESS);
		ASSERT_EQ((size_t)data_ptr, i);
	}

	vector_destroy(vector);
}

TEST(SPSC, Push_Pop)
{
	mpmc_simulate(mpmc_sim_opt_t {
		.vector_size = 1,
			.data_amount = 1,
			.producers_n = 1,
			.consumers_n = 1,
			.producer_sleep = 0,
			.consumer_sleep = 1
	});
}

TEST(SPSC, Pop_Block_Push)
{
	mpmc_simulate(mpmc_sim_opt_t {
		.vector_size = 1,
			.data_amount = 1,
			.producers_n = 1,
			.consumers_n = 1,
			.producer_sleep = 1,
			.consumer_sleep = 0
	});
}

TEST(SPSC, FullVector_Overflow)
{
	mpmc_simulate(mpmc_sim_opt_t {
		.vector_size = 10,
			.data_amount = 50,
			.producers_n = 1,
			.consumers_n = 1,
			.producer_sleep = 0,
			.consumer_sleep = 1
	});
}

TEST(SPMC, Push_Pop)
{
	mpmc_simulate(mpmc_sim_opt_t {
		.vector_size = 20,
			.data_amount = 20,
			.producers_n = 1,
			.consumers_n = 5,
			.producer_sleep = 0,
			.consumer_sleep = 1
	});
}

TEST(SPMC, Pop_Block_Push)
{
	mpmc_simulate(mpmc_sim_opt_t {
		.vector_size = 20,
			.data_amount = 20,
			.producers_n = 1,
			.consumers_n = 5,
			.producer_sleep = 1,
			.consumer_sleep = 0
	});
}

TEST(SPMC, FullVector_Overflow)
{
	mpmc_simulate(mpmc_sim_opt_t {
		.vector_size = 10,
			.data_amount = 50,
			.producers_n = 1,
			.consumers_n = 5,
			.producer_sleep = 0,
			.consumer_sleep = 1
	});
}

TEST(MPSC, Push_Pop)
{
	mpmc_simulate(mpmc_sim_opt_t {
		.vector_size = 20,
			.data_amount = 20,
			.producers_n = 5,
			.consumers_n = 1,
			.producer_sleep = 0,
			.consumer_sleep = 1
	});
}

TEST(MPSC, Pop_Block_Push)
{
	mpmc_simulate(mpmc_sim_opt_t {
		.vector_size = 20,
			.data_amount = 20,
			.producers_n = 5,
			.consumers_n = 1,
			.producer_sleep = 1,
			.consumer_sleep = 0
	});
}

TEST(MPSC, FullVector_Overflow)
{
	mpmc_simulate(mpmc_sim_opt_t {
		.vector_size = 10,
			.data_amount = 50,
			.producers_n = 5,
			.consumers_n = 1,
			.producer_sleep = 0,
			.consumer_sleep = 1
	});
}

TEST(MPMC, Push_Pop)
{
	mpmc_simulate(mpmc_sim_opt_t {
		.vector_size = 20,
			.data_amount = 20,
			.producers_n = 5,
			.consumers_n = 5,
			.producer_sleep = 0,
			.consumer_sleep = 1
	});
}

TEST(MPMC, Pop_Block_Push)
{
	mpmc_simulate(mpmc_sim_opt_t {
		.vector_size = 20,
			.data_amount = 20,
			.producers_n = 5,
			.consumers_n = 5,
			.producer_sleep = 1,
			.consumer_sleep = 0
	});
}

TEST(MPMC, FullVector_Overflow)
{
	mpmc_simulate(mpmc_sim_opt_t {
		.vector_size = 10,
			.data_amount = 20,
			.producers_n = 5,
			.consumers_n = 5,
			.producer_sleep = 1,
			.consumer_sleep = 0
	});
}

// Recursive function to return gcd of a and b
long long gcd(long long int a, long long int b)
{
	if (b == 0)
		return a;
	return gcd(b, a % b);
}

// Function to return LCM of two numbers
long long lcm(int a, int b)
{
	return (a / gcd(a, b)) * b;
}

int my_accumulate(int* result, size_t n) {
	return std::accumulate(result, result + n, 0);
}

void mpmc_simulate(mpmc_sim_opt_t options)
{
	size_t vector_size = options.vector_size;
	size_t data_amount = options.data_amount;
	size_t producers_n = options.producers_n; 
	size_t consumers_n = options.consumers_n;
	size_t producer_sleep = options.producer_sleep;
	size_t consumer_sleep = options.consumer_sleep;

	// we need to be sure that data_amount is divisible by both producers_n and consumers_n
	long long alignment = lcm(producers_n, consumers_n);
	if (data_amount % alignment != 0) {
		data_amount += (alignment - data_amount % alignment);
	}

	vector_t* vector = vector_create(vector_size);

	// consumers_result[i] -- the data popped by consumer 'i'
	int* consumers_result = new int[consumers_n];
	memset(consumers_result, 0, consumers_n * sizeof(int));

	// producers_result[i] -- the data pushed by producer 'i'
	int* producers_result = new int[producers_n];
	memset(producers_result, 0, producers_n * sizeof(int));

	// Vector contains thread IDs
	std::vector<std::thread> producers;
	std::vector<std::thread> consumers;

	// create producers_n amount of producer threads
	for (size_t thread_n = 0; thread_n < producers_n; thread_n++) {
		producers.push_back(std::thread([=, &producers_result]() {

			sleep(producer_sleep);

		for (size_t iter = 0; iter < (unsigned int)((double)data_amount / (double)producers_n); iter++) {
			EXPECT_EQ(vector_push(vector, (void*)(thread_n * iter)), VECTOR_SUCCESS);
			producers_result[thread_n] += (int)(thread_n * iter);
		}

			})
		);
	}

	// create consumers_n amount of consumer threads
	for (size_t thread_n = 0; thread_n < consumers_n; thread_n++) {
		consumers.push_back(std::thread([=, &consumers_result]() {

			void* data_ptr = nullptr;

		sleep(consumer_sleep);

		for (size_t iter = 0; iter < (unsigned int)((double)data_amount / (double)consumers_n); iter++) {
			EXPECT_EQ(vector_pop(vector, &data_ptr), VECTOR_SUCCESS);
			consumers_result[thread_n] += (intptr_t) data_ptr;
		}

			})
		);
	}

	// Join producers
	std::for_each(producers.begin(), producers.end(), [](std::thread& t1) { t1.join(); });

	// Join consumers
	std::for_each(consumers.begin(), consumers.end(), [](std::thread& t2) { t2.join(); });

	vector_destroy(vector);

	// Compare the sum of pushed and popped data
	EXPECT_EQ(my_accumulate(producers_result, producers_n), my_accumulate(consumers_result, consumers_n));

	delete[] producers_result;
	delete[] consumers_result;
}