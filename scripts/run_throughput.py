#!/usr/bin/python
import os
import sys
import bandwidth

def main():
    delays = [1, 2]
    bw_begin = 8
    bw_end = 80
    bw_step = 8
    num_iterations = 200
    policy_indicator = 56
    time_seed = 1
    qos_x = 1
    qos_y = 1
    qos_ratio = 0
    save_config_and_deficit = 0
    output_throughput = 1
    input_file = "input.txt"
    stability_file_prefix = "stability"
    bandwidth.word_writer(input_file, 8, 3, qos_x)
    bandwidth.word_writer(input_file, 8, 4, qos_y)
    bandwidth.word_writer(input_file, 9, 4, qos_ratio)
    bandwidth.word_writer(input_file, 10, 4, num_iterations)
    bandwidth.word_writer(input_file, 11, 3, policy_indicator)
    bandwidth.word_writer(input_file, 12, 3, time_seed)
    bandwidth.word_writer(input_file, 13, 5, save_config_and_deficit)
    bandwidth.word_writer(input_file, 14, 3, output_throughput)
    for delay in delays:
        bandwidth.word_writer(input_file, 5, 5, delay)
        bandwidth.word_writer(input_file, 6, 5, delay)
        directory = "throughput/delay"+str(delay)+"/"
        if not os.path.exists(directory):
            os.makedirs(directory)
        for bw in bandwidth.my_range(bw_begin, bw_end, bw_step):
            bandwidth.word_writer(input_file, 7, 2, bw)
            cdin = Chdir(directory)
            os.system("nice time ../../real-time-scheduler ../../"+input_file+" ../../"+network_file+" "+stability_file_prefix)
            cdin = Chdir("../../")
            print "Done with bandwidth", bw, "delay", delay

class Chdir:
    def __init__( self, newPath ):
        self.savedPath = os.getcwd()
        os.chdir(newPath)
    def __del__( self ):
        os.chdir( self.savedPath )

if __name__ == "__main__":
    main()
