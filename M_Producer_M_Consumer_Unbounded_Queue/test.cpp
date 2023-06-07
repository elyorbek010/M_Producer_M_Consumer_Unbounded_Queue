extern "C" {
#include "vector.h"
}

#include "gtest/gtest.h"
#include <thread>
#include <vector>
#include <algorithm>
#include <numeric>

#ifdef _WIN32
#include <Windows.h>
void sleep(int sec) { Sleep(sec * 1000); } // Make function sleep(), compatible with unix
#else
#include <unistd.h>
#endif

/* Call functions with invalid(NULL) pointers*/
TEST(SPSC, NULL_INPUT_TEST) {
	vector_t* vector;

	vector = vector_create(5);

	int val = 0;

	EXPECT_EQ(vector_destroy(nullptr), VECTOR_FAILURE);
	EXPECT_EQ(vector_push(nullptr, &val), VECTOR_FAILURE);
	EXPECT_EQ(vector_push(nullptr, &val), VECTOR_FAILURE);

	EXPECT_EQ(vector_pop(vector, nullptr), VECTOR_FAILURE);
	EXPECT_EQ(vector_pop(vector, nullptr), VECTOR_FAILURE);

	vector_destroy(vector);
}

/*
* Smoke test testing basic capabilities of the vector without corner cases
* Create Vector
* Push element
* Pop element
* Destroy vector
*/
TEST(SPSC, Smoke_Test_Vector)
{
	vector_t* vector = nullptr;

	int var1 = 10;
	void* var2 = NULL;

	vector = vector_create(5);

	EXPECT_EQ(vector_push(vector, &var1), VECTOR_SUCCESS);

	EXPECT_EQ(vector_pop(vector, &var2), VECTOR_SUCCESS);

	EXPECT_EQ(&var1, var2);

	vector_destroy(vector);
}

/*Main: Push, Thread: Pop and vice versa*/
TEST(SPSC, Thread_Smoke_Test_Vector)
{
	vector_t* vector = nullptr;
	void* m_ptr = nullptr;

	vector = vector_create(3);

	std::thread t1([&vector]() {
		void* t_ptr = nullptr;

		sleep(1);

		EXPECT_EQ(vector_pop(vector, &t_ptr), VECTOR_SUCCESS);
		EXPECT_EQ(vector_push(vector, (void*)200), VECTOR_SUCCESS);

		EXPECT_EQ((int)t_ptr, 100);	// verify the value
	});

	EXPECT_EQ(vector_push(vector, (void*)100), VECTOR_SUCCESS);

	sleep(2);

	EXPECT_EQ(vector_pop(vector, &m_ptr), VECTOR_SUCCESS);

	EXPECT_EQ((int)m_ptr, 200); // verify the value

	t1.join();

	vector_destroy(vector);
}

/*
* When queue overflows, it must allocate more space to fit in the new data
*/
TEST(SPSC, FullQueue_Push)
{
	vector_t* vector = nullptr;
	float var1 = 5;
	void* var2;

	vector = vector_create(1);

	// push values
	ASSERT_EQ(vector_push(vector, &var1 + 0), VECTOR_SUCCESS); // 1/1 element

	EXPECT_EQ(vector_push(vector, &var1 + 1), VECTOR_SUCCESS); // enlarge 2/2 element
	EXPECT_EQ(vector_push(vector, &var1 + 2), VECTOR_SUCCESS); // enlarge 3/4 element

	ASSERT_EQ(vector_push(vector, &var1 + 3), VECTOR_SUCCESS); // 4/4 element

	EXPECT_EQ(vector_push(vector, &var1 + 4), VECTOR_SUCCESS); // enlarge 5/8 element

	// pop values
	ASSERT_EQ(vector_pop(vector, &var2), VECTOR_SUCCESS);
	EXPECT_EQ(var2, &var1 + 0);

	ASSERT_EQ(vector_pop(vector, &var2), VECTOR_SUCCESS);
	EXPECT_EQ(var2, &var1 + 1);

	ASSERT_EQ(vector_pop(vector, &var2), VECTOR_SUCCESS);
	EXPECT_EQ(var2, &var1 + 2);

	ASSERT_EQ(vector_pop(vector, &var2), VECTOR_SUCCESS);
	EXPECT_EQ(var2, &var1 + 3);

	ASSERT_EQ(vector_pop(vector, &var2), VECTOR_SUCCESS);
	EXPECT_EQ(var2, &var1 + 4);

	vector_destroy(vector);
}

/*
* Check that values can be inserted without blocking or waiting if enough space exists
*/
TEST(SPSC, Circulation)
{
	vector_t* vector = nullptr;
	float var1 = 5;
	void* var2;

	vector = vector_create(3);

	// push, vector elem 3/3
	ASSERT_EQ(vector_push(vector, &var1 + 0), VECTOR_SUCCESS);
	ASSERT_EQ(vector_push(vector, &var1 + 1), VECTOR_SUCCESS);
	ASSERT_EQ(vector_push(vector, &var1 + 2), VECTOR_SUCCESS);

	// pop, vector elem 1/3
	ASSERT_EQ(vector_pop(vector, &var2), VECTOR_SUCCESS);
	EXPECT_EQ(var2, &var1 + 0);
	ASSERT_EQ(vector_pop(vector, &var2), VECTOR_SUCCESS);
	EXPECT_EQ(var2, &var1 + 1);

	// push, vector elem 3/3
	EXPECT_EQ(vector_push(vector, &var1 + 3), VECTOR_SUCCESS);
	EXPECT_EQ(vector_push(vector, &var1 + 4), VECTOR_SUCCESS);

	// pop values
	ASSERT_EQ(vector_pop(vector, &var2), VECTOR_SUCCESS);
	EXPECT_EQ(var2, &var1 + 2);

	ASSERT_EQ(vector_pop(vector, &var2), VECTOR_SUCCESS);
	EXPECT_EQ(var2, &var1 + 3);

	ASSERT_EQ(vector_pop(vector, &var2), VECTOR_SUCCESS);
	EXPECT_EQ(var2, &var1 + 4);

	vector_destroy(vector);
}

TEST(SPSC, EmptyQueue_Pop)
{
	vector_t* vector = nullptr;
	void* ptr = nullptr;

	vector = vector_create(3);

	std::thread t1([&]() {
		EXPECT_EQ(vector_pop(vector, &ptr), VECTOR_SUCCESS);
		});

	sleep(1);

	ASSERT_EQ(ptr, nullptr); // since thread is blocked, nobody changed the value of ptr

	EXPECT_EQ(vector_push(vector, (void*)100), VECTOR_SUCCESS);

	t1.join();

	EXPECT_EQ((int)ptr, 100); // after thread termination we can check the value

	vector_destroy(vector);
}

TEST(SPMC, Concurrency)
{
	const size_t num_of_clients = 5;
	size_t n = num_of_clients * 5;
	int result[num_of_clients] = { 0 };

	vector_t* vector = vector_create(n);

	std::vector<std::thread> clients;
	for (size_t i = 0; i < num_of_clients; i++) {							// create num_of_clients amount of threads
		{
			clients.push_back(std::thread([i, vector, &result]() {			// save thread id in clients vector to join them later
				
				void* data = nullptr;

				for (size_t j = 0; j < 5; j++) {							// vector_pop 5 times in each thread
					EXPECT_EQ(vector_pop(vector, &data), VECTOR_SUCCESS);
					result[i] += (int)data;									// save data in shared pool
				}

			}));
		}
	}

	for (size_t i = 0; i < n; i++) {									// push num_of_clients * 5 amount of data
		ASSERT_EQ(vector_push(vector, (void*)i), VECTOR_SUCCESS);
		if (rand() % 2 == 0)											// sleep sometimes
			sleep(1);
	}

	std::for_each(clients.begin(), clients.end(), [](std::thread& t)	// join all threads
		{
			t.join();
		});

	vector_destroy(vector);

	int sum_popped = 0;
	sum_popped = std::accumulate(result, result + num_of_clients, 0);	// calculate the sum of all popped values

	EXPECT_EQ(sum_popped, (n - 1) * n / 2);	 // check if the sum of all popped data is equal to the sum of all pushed data
}

TEST(MPSC, VectorOverflow)
{
	const size_t num_of_clients = 5;
	size_t n = num_of_clients * 5;
	int result[num_of_clients] = { 0 };

	vector_t* vector = vector_create(n / 2);	// vector will enlarge to fit in the new data

	std::vector<std::thread> clients;
	for (size_t i = 0; i < num_of_clients; i++) {							// create num_of_clients amount of threads
		{
			clients.push_back(std::thread([i, vector, &result]() {			// save thread id in clients vector to join them later

				for (size_t j = 0; j < 5; j++) {							// vector_push 5 times in each thread
					EXPECT_EQ(vector_push(vector, (void *)(i * j)), VECTOR_SUCCESS);
					result[i] += (int)(i * j);								// save data in shared pool
					if (rand() % 2 == 0)										
						sleep(1);											// sleep sometimes
				}

			}));
		}
	}

	std::for_each(clients.begin(), clients.end(), [](std::thread& t)	// join all threads
		{
			t.join();
		});

	void* data = nullptr;
	int sum_popped = 0;
	for (size_t i = 0; i < n; i++) {									// pop num_of_clients * 5 amount of data
		ASSERT_EQ(vector_pop(vector, &data), VECTOR_SUCCESS);
		sum_popped += (int)data;
	}

	vector_destroy(vector);

	int sum_pushed = std::accumulate(result, result + num_of_clients, 0); // calculate the sum of all popped values

	EXPECT_EQ(sum_popped, sum_pushed);	 // check if the sum of all popped data is equal to the sum of all pushed data
}

TEST(MPSC, Concurrency)
{
	const size_t num_of_clients = 5;
	size_t n = num_of_clients * 5;
	int result[num_of_clients] = { 0 };

	vector_t* vector = vector_create(n / 2);	// vector will enlarge to fit in the new data

	std::vector<std::thread> clients;
	for (size_t i = 0; i < num_of_clients; i++) {							// create num_of_clients amount of threads
		{
			clients.push_back(std::thread([i, vector, &result]() {			// save thread id in clients vector to join them later

				for (size_t j = 0; j < 5; j++) {							// vector_push 5 times in each thread
					EXPECT_EQ(vector_push(vector, (void*)(i * j)), VECTOR_SUCCESS);
					result[i] += (int)(i * j);								// save data in shared pool
					if (rand() % 2 == 0)
						sleep(1);											// sleep sometimes
				}

				}));
		}
	}

	void* data = nullptr;
	int sum_popped = 0;
	for (size_t i = 0; i < n; i++) {									// pop num_of_clients * 5 amount of data
		ASSERT_EQ(vector_pop(vector, &data), VECTOR_SUCCESS);
		sum_popped += (int)data;
	}

	std::for_each(clients.begin(), clients.end(), [](std::thread& t)	// join all threads
		{
			t.join();
		});

	vector_destroy(vector);

	int sum_pushed = std::accumulate(result, result + num_of_clients, 0); // calculate the sum of all popped values

	EXPECT_EQ(sum_popped, sum_pushed);	 // check if the sum of all popped data is equal to the sum of all pushed data
}

TEST(MPMC, Concurrency)
{
	const size_t num_of_threads = 5;
	size_t n = num_of_threads * 5;
	int result_clients[num_of_threads] = { 0 };
	int result_producers[num_of_threads] = { 0 };

	vector_t* vector = vector_create(n / 2);	// vector will enlarge to fit in the new data

	std::vector<std::thread> clients;
	for (size_t i = 0; i < num_of_threads; i++) {									// create num_of_clients amount of threads
		{
			clients.push_back(std::thread([i, vector, &result_clients]() {			// save thread id in clients vector to join them later

				void* data = nullptr;

				for (size_t j = 0; j < 5; j++) {									// vector_pop 5 times in each thread
					EXPECT_EQ(vector_pop(vector, &data), VECTOR_SUCCESS);
					result_clients[i] += (int)data;								// save data in shared pool
					if (rand() % 2 == 0)
						sleep(1);													// sleep sometimes
				}

			}));
		}
	}

	std::vector<std::thread> producers;
	for (size_t i = 0; i < num_of_threads; i++) {									// create num_of_clients amount of threads
		{
			producers.push_back(std::thread([i, vector, &result_producers]() {		// save thread id in producers vector to join them later

				for (size_t j = 0; j < 5; j++) {									// vector_push 5 times in each thread
					EXPECT_EQ(vector_push(vector, (void*)(i * j)), VECTOR_SUCCESS);
					result_producers[i] += (int)(i * j);							// save data in shared pool
					if (rand() % 2 == 0)
						sleep(1);													// sleep sometimes
				}

			}));
		}
	}

	std::for_each(clients.begin(), clients.end(), [](std::thread& t)		// join producers
		{
			t.join();
		});

	std::for_each(producers.begin(), producers.end(), [](std::thread& t)	// join clients
		{
			t.join();
		});

	vector_destroy(vector);

	int sum_producers = std::accumulate(result_producers, result_producers + num_of_threads, 0);// calculate the sum of all pushed values
	int sum_clients = std::accumulate(result_clients, result_clients + num_of_threads, 0);		// calculate the sum of all popped values

	EXPECT_EQ(sum_producers, sum_clients);	 // check if the sum of all popped data is equal to the sum of all pushed data
}