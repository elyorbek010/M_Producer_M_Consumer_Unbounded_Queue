# M_Producer_M_Consumer_Unbounded_Queue
Last Test Results:

<pre>
2023-06-15T08:25:52+00:00
Running ./MpmcBenchmark
Run on (12 X 2894.56 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x12)
  L1 Instruction 32 KiB (x12)
  L2 Unified 512 KiB (x12)
  L3 Unified 8192 KiB (x1)
Load Average: 0.41, 0.52, 0.25
***WARNING*** Library was built as DEBUG. Timings may be affected.
---------------------------------------------------------------------------
Benchmark                                 Time             CPU   Iterations
---------------------------------------------------------------------------
Mpmc_Bench_spsc_simulate/1            91450 ns        70155 ns         9880
Mpmc_Bench_spsc_simulate/2            93083 ns        70892 ns         9754
Mpmc_Bench_spsc_simulate/4            94175 ns        72086 ns         9586
Mpmc_Bench_spsc_simulate/8            95245 ns        72828 ns         9228
Mpmc_Bench_spsc_simulate/16           97141 ns        74139 ns         8544
Mpmc_Bench_spsc_simulate/32           94681 ns        71008 ns         9566
Mpmc_Bench_spsc_simulate/64           95464 ns        73325 ns         9938
Mpmc_Bench_spsc_simulate/128          98121 ns        74643 ns         9222
Mpmc_Bench_spsc_simulate/256         105070 ns        74890 ns         9785
Mpmc_Bench_spsc_simulate/512         130259 ns        81235 ns         8102
Mpmc_Bench_spsc_simulate/1024        229619 ns       108083 ns         6386
Mpmc_Bench_spsc_simulate/2048        410896 ns       128157 ns         5735
Mpmc_Bench_spsc_simulate/4096        809113 ns       143179 ns         4948
Mpmc_Bench_spsc_simulate/8192       1273639 ns       127216 ns         3924
Mpmc_Bench_spsc_simulate/16384      2486333 ns       139572 ns         4906
Mpmc_Bench_spsc_simulate/32768      4992034 ns       161164 ns         1000
Mpmc_Bench_spsc_simulate/65536     10036854 ns       197881 ns         1000
Mpmc_Bench_spsc_simulate/131072    20199482 ns       190577 ns         1000
Mpmc_Bench_spsc_simulate/262144    42006283 ns       184393 ns          100
Mpmc_Bench_spsc_simulate/524288    94421639 ns       243632 ns          100
Mpmc_Bench_spsc_simulate/1048576  178806675 ns       282576 ns          100
</pre>
