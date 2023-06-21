extern "C"
{
#include "../vector.h"
}

#include <benchmark/benchmark.h>
#include <thread>
#include <iostream>
#include <fstream>
#include <chrono>
#include <string>

#define LOG_ENABLED 0

void spsc_simulate(size_t vector_size, size_t data_amount)
{
  vector_t *vector = vector_create(vector_size);

  auto producer = std::thread([=]()
                              {
                                vector_ret_t ret = VECTOR_SUCCESS;
                                size_t iter = 0;
#if LOG_ENABLED == 1
                                std::string filename = "producer_logs_" + std::to_string(data_amount) + ".txt";

                                std::ofstream producer_logs;

                                producer_logs.open(filename, std::ios::out | std::ios::trunc);
                                if (!producer_logs.is_open())
                                {
                                  std::cout << "error opening" << filename << std::endl;
                                }
#endif
                                while (iter < data_amount)
                                {
                                  ret = vector_push(vector, (void *)iter);

                                  if (ret == VECTOR_FAILURE)
                                  {
                                    abort();
                                  }
                                  else if (ret == VECTOR_SUCCESS)
                                  {
#if LOG_ENABLED == 1
                                    producer_logs << "PUSHED "
                                                  << iter
                                                  << " "
                                                  << std::chrono::system_clock::now().time_since_epoch() /
                                                         std::chrono::nanoseconds(1)
                                                  << "\n";
#endif
                                    iter++;
                                  }
                                }
#if LOG_ENABLED == 1
                                producer_logs.close();
#endif
                              });

  auto consumer = std::thread([=]()
                              {
                                vector_ret_t ret = VECTOR_SUCCESS;
                                void *data_ptr = nullptr;
                                size_t iter = 0;
#if LOG_ENABLED == 1
                                std::string filename = "consumer_logs_" + std::to_string(data_amount) + ".txt";

                                std::ofstream consumer_logs;
                                consumer_logs.open(filename, std::ios::out | std::ios::trunc);
#endif
                                while (iter < data_amount)
                                {
                                  ret = vector_pop(vector, &data_ptr);
#if LOG_ENABLED == 1
                                  consumer_logs << "POPPED "
                                                << " "
                                                << (size_t)data_ptr
                                                << std::chrono::system_clock::now().time_since_epoch() /
                                                       std::chrono::nanoseconds(1)
                                                << "\n";
#endif
                                  if (ret == VECTOR_FAILURE)
                                  {
                                    abort();
                                  }
                                  else if (ret == VECTOR_SUCCESS)
                                    iter++;
                                }
#if LOG_ENABLED == 1
                                consumer_logs.close();
#endif
                              });

  // join threads
  producer.join();
  consumer.join();

  vector_destroy(vector);
}

static void Bench_spsc_simulate(benchmark::State &state)
{
  for (auto _ : state)
  {
    spsc_simulate(1000, state.range(0));
  }
}

BENCHMARK(Bench_spsc_simulate)->RangeMultiplier(2)->Range(1, 1 << 20);

BENCHMARK_MAIN();