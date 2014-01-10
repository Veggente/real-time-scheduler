#! /usr/bin/python
import bandwidth
import os
input_file = "find_max_qos_ratio.py"
bw_start = 24
bw_end = 72
bw_step = 24
for bw in bandwidth.my_range(bw_start, bw_end, bw_step):
    bandwidth.word_writer(input_file, 10, 3, bw)
    os.system("python "+input_file)
