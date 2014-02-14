real-time-scheduler
===================

A time-slotted network simulator for scheduling packets with hard deadlines.

Compilation
===========
- To compile on OS X 10.9:

```bash
cd real-time-scheduler/
make
```

- To compile on OS X 10.7:

```bash
cd real-time-scheduler/
make old
```

- To compile on Linux with `g++4.6` or higher

```bash
cd real-time-scheduler/
make g++
```

The default name of the compilation output is `real-time-scheduler`

Quickstart
==========

```bash
./real-time-scheduler input_file.txt network_file.txt stability_output_prefix
```
where ``input_file.txt`` specifies the configuration of the simulation, ``network_file.txt`` gives the network (only necessary when *Network type* in ``input_file.txt`` is `unit-disk`), and ``stability_output_prefix`` is the prefix for the stability statistics output.

An example ``input_file.txt`` is:

```
Network type: unit-disk
Topological parameters: 2
Arrival distribution: bernoulli
Arrival parameters: 1 10 0.5 1 10 0.5
Min delay bound: 1 2
Max delay bound: 1 2
Bandwidths: 1 2
Base QoS: 1 1
Ratios of QoS: 0.5 0.6
Number of iterations: 1000
Policy indicator: 3
Timed seed: 1
Save config and deficits: 1
Output throughput: 1
```
Here are some notes:

- *Network type* can be `collocated`, `line`, `cycle` or `unit-disk`.
- *Topological parameters* are network size for `collocated`, network size and interference radius for `line` and `cycle`, and irrelevant for `unit-disk` since it will be overwritten by ``network_file.txt``.
- *Arrival distribution* can be `uniform`, `binomial` or `bernoulli`.
- *Arrival parameters* are two sets of parameters for the arrival distribution. Half random links use one set and the other half use the other set.
- *Min delay bound* are two sets of minimum delay bounds of packets. They are randomly assigned to the links.
- *Max delay bound* are two sets of maximum delay bounds of packets. They are assigned to the corresponding links as for *Min delay bound*.
- *Bandwidths* are a sequence of bandwidths/speedups the systems operate under. A bandwidth/speedup of k means each link can potentially transmit k packets instead of 1 in each time slot.
- *Base QoS* is the reference QoS vector of two components. Each component is assigned to random half of the links.
- *Ratios of QoS* is a sequence of ratios which we multiply to the base QoS to get the actual QoS we use.
- *Number of iterations* is the number of time slots in the simulation.
- *Policy indicator* specifies which of the policies to use. For example 3 = 000011(base 2) means only the first two policies (LDF and EDF) are tested.
- *Timed seed* is 1 if the random number generator uses timed seed, and 0 if it uses fixed seed.
- *Save config and deficits* is 1 if the configuration of the simulation and the deficits for each time slots and each link are outputted, and 0 otherwise.
- *Output throughput* is 1 if the throughput is included in the stability output files, and 0 otherwise.

An example ``network_file.txt`` is:

```
Number of links: 20
Number of maximal schedules: 66
Maximal schedules:
1 2 3 20 
1 3 4 
1 3 6 20 
1 3 11 
1 3 12 
1 3 13 
1 3 17 20 
1 3 19 20 
1 4 18 
1 9 11 
1 9 13 
1 9 20 
1 11 18 
1 12 18 
1 13 18 
1 18 20 
2 3 14 20 
3 4 5 14 
3 4 7 14 
3 4 8 14 
3 4 10 
3 4 14 15 
3 4 14 16 
3 5 6 14 20 
3 5 12 14 
3 6 14 16 20 
3 7 12 14 
3 7 14 20 
3 8 12 14 
3 8 14 20 
3 10 12 
3 10 20 
3 11 14 16 
3 12 14 15 
3 12 14 16 
3 13 14 
3 14 15 20 
3 14 17 20 
3 14 19 20 
4 5 14 18 
4 7 14 18 
4 8 14 18 
4 10 18 
4 14 15 18 
4 14 16 18 
5 9 14 20 
5 12 14 18 
5 14 18 20 
7 9 14 20 
7 12 14 18 
7 14 18 20 
8 12 14 18 
8 14 18 20 
9 10 20 
9 11 14 16 
9 13 14 
9 14 15 20 
9 14 16 20 
10 12 18 
10 18 20 
11 14 16 18 
12 14 15 18 
12 14 16 18 
13 14 18 
14 15 18 20 
14 16 18 20 
```
Here each line starting from line 4 is a maximum independent set.

The output files in this example are

```
un20-it1000-ldf-b2-x1-y1-r0.6.txt
un20-it1000-ldf-b2-x1-y1-r0.5.txt
un20-it1000-ldf-b1-x1-y1-r0.6.txt
un20-it1000-ldf-b1-x1-y1-r0.5.txt
un20-it1000-edf-b2-x1-y1-r0.6.txt
un20-it1000-edf-b2-x1-y1-r0.5.txt
un20-it1000-edf-b1-x1-y1-r0.6.txt
un20-it1000-edf-b1-x1-y1-r0.5.txt
stability_output_prefix-ldf-b2.txt
stability_output_prefix-ldf-b1.txt
stability_output_prefix-edf-b2.txt
stability_output_prefix-edf-b1.txt
```
where the `un*` files contain the configuration and deficits, and `stability*` files are the stability output files which contain the stability ratios for stability check.


