extern "C"
{
#include "../vector.h"
}

#include <benchmark/benchmark.h>
#include <thread>

void spsc_simulate(size_t vector_size, size_t data_amount)
{
  vector_t *vector = vector_create(vector_size);

  auto producer = std::thread([=]()
                              {
                                vector_ret_t ret = VECTOR_SUCCESS;
                                size_t iter = 0;

                                while (iter < data_amount)
                                {
                                  ret = vector_push(vector, (void *)iter);

                                  if (ret == VECTOR_FAILURE)
                                    abort();
                                  else if (ret == VECTOR_SUCCESS) 
                                    iter++;
                                } });

  auto consumer = std::thread([=]()
                              {
                                vector_ret_t ret = VECTOR_SUCCESS;
                                void *data_ptr = nullptr;
                                size_t iter = 0;

                                while (iter < data_amount)
                                {
                                  ret = vector_pop(vector, &data_ptr);

                                  if (ret == VECTOR_FAILURE)
                                    abort();
                                  else if (ret == VECTOR_SUCCESS)
                                    iter++;
                                } });

  // join threads
  producer.join();
  consumer.join();

  vector_destroy(vector);
}

static void Mpmc_Bench_spsc_simulate(benchmark::State &state)
{
  for (auto _ : state)
  {
    spsc_simulate(1000, state.range(0));
  }
}

BENCHMARK(Mpmc_Bench_spsc_simulate)->RangeMultiplier(2)->Range(1, 1 << 20);

BENCHMARK_MAIN();