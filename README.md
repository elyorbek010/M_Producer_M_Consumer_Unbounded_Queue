# M_Producer_M_Consumer_Unbounded_Queue
Last Test Results:

<pre>
2023-06-20T18:33:48+00:00
Running ./MpmcBenchmark
Run on (8 X 2894.56 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x8)
  L1 Instruction 32 KiB (x8)
  L2 Unified 512 KiB (x8)
  L3 Unified 8192 KiB (x1)
Load Average: 0.38, 0.38, 0.20
***WARNING*** Library was built as DEBUG. Timings may be affected.
---------------------------------------------------------------------------
Benchmark                                 Time             CPU   Iterations
---------------------------------------------------------------------------
Mpmc_Bench_spsc_simulate/1            48160 ns        38579 ns        17283
Mpmc_Bench_spsc_simulate/2            47306 ns        37713 ns        17529
Mpmc_Bench_spsc_simulate/4            53420 ns        42275 ns        16933
Mpmc_Bench_spsc_simulate/8            50350 ns        40183 ns        17694
Mpmc_Bench_spsc_simulate/16           50272 ns        39887 ns        17587
Mpmc_Bench_spsc_simulate/32           49459 ns        39664 ns        17639
Mpmc_Bench_spsc_simulate/64           49788 ns        39683 ns        18299
Mpmc_Bench_spsc_simulate/128          49081 ns        38549 ns        18059
Mpmc_Bench_spsc_simulate/256          50861 ns        38359 ns        17815
Mpmc_Bench_spsc_simulate/512          62452 ns        41703 ns        17619
Mpmc_Bench_spsc_simulate/1024        111808 ns        45553 ns        15196
Mpmc_Bench_spsc_simulate/2048        219766 ns        51081 ns        13767
Mpmc_Bench_spsc_simulate/4096        469315 ns        59757 ns        11573
Mpmc_Bench_spsc_simulate/8192       1008217 ns        74595 ns         9306
Mpmc_Bench_spsc_simulate/16384      2243682 ns        95668 ns         6930
Mpmc_Bench_spsc_simulate/32768      4667650 ns       118388 ns         1000
Mpmc_Bench_spsc_simulate/65536      9118971 ns       132405 ns         1000
Mpmc_Bench_spsc_simulate/131072    18756974 ns       158035 ns         1000
Mpmc_Bench_spsc_simulate/262144    37241349 ns       174083 ns          100
Mpmc_Bench_spsc_simulate/524288    75749726 ns       191195 ns          100
Mpmc_Bench_spsc_simulate/1048576  153391734 ns       230662 ns          100
</pre>
