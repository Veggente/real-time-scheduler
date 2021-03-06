#!/usr/bin/python
import sys
import getopt
import bandwidth
import os

class Chdir:
    def __init__( self, newPath ):
        self.savedPath = os.getcwd()
        os.chdir(newPath)
    def __del__( self ):
        os.chdir( self.savedPath )

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "x:y:d:b:e:s:p:i:t")
    except getopt.GetoptError as err:
        # Print help information and exit:
        print str(err)  # Will print something like "option -a not recognized".
        sys.exit(2)
    qos_x = "1"
    qos_y = "1"
    delay_bound = "1"
    bw_begin = 1
    bw_end = 1
    bw_step = 1
    policy = "ldf"
    input_file = "input_file.txt"
    save_config_and_deficit = 1
    for o, a in opts:
        if o == "-x":
            qos_x = a
        elif o == "-y":
            qos_y = a
        elif o == "-d":
            delay_bound = a
        elif o == "-b":
            bw_begin = int(float(a))
        elif o == "-e":
            bw_end = int(float(a))
        elif o == "-s":
            bw_step = int(float(a))
        elif o == "-p":
            policy = a
        elif o == "-i":
            input_file = a
        elif o == "-t":
            save_config_and_deficit = 0
        else:
            assert False, "unhandled option"

    # Other parameters.
    qos_ratio_initial = 0.5  # Initial ratio.
    max_trial = 15           # Maximum number of trials before terminating.
    qos_step = 0.001         # Step size.
    num_iterations = 20000   # Number of iterations.
    min_ratio = 0.0
    max_ratio = 1.0
    network_file = "network_file.txt"
    stability_file_prefix = "stability"
    time_seed = 1

    # Initialization.
    bandwidth.word_writer(input_file, 8, 3, qos_x) 
    bandwidth.word_writer(input_file, 8, 4, qos_y)        # Set base QoS.
    bandwidth.word_writer(input_file, 5, 5, delay_bound)  # Set delay bound.
    bandwidth.word_writer(input_file, 6, 5, delay_bound)
    directory = "qos-x"+qos_x+"-y"+qos_y+"/delay_bound_"+delay_bound+"/"
    policy_indicator = 1
    if not os.path.exists(directory):
        os.makedirs(directory)  # Make directory.
    if policy == "ldf":
        policy_indicator = 1
    elif policy == "edf":
        policy_indicator = 2
    elif policy == "sdbf":
        policy_indicator = 4
    elif policy == "edf-naive":
        policy_indicator = 8
    elif policy == "sdbf-naive":
        policy_indicator = 16
    elif policy == "maximal":
        policy_indicator = 32
    elif policy == "ldf-threshold":
        policy_indicator = 64
    elif policy == "ldf-vision":
        policy_indicator = 128
    elif policy == "max-deficit":
        policy_indicator = 256
    else:
        print "Please use a valid policy."
        exit(2)
    # Set policy indicator.
    bandwidth.word_writer(input_file, 11, 3, policy_indicator)
    # Set time seed.
    bandwidth.word_writer(input_file, 12, 3, 1)
    bandwidth.word_writer(input_file, 13, 5, save_config_and_deficit)
    # Set number of iterations.
    bandwidth.word_writer(input_file, 10, 4, num_iterations)
    # For bandwidth loop.
    for bw in bandwidth.my_range(bw_begin, bw_end, bw_step):
        qos_ratio = qos_ratio_initial
        bandwidth.word_writer(input_file, 7, 2, bw)  # Set bandwidth.
        stability_file = stability_file_prefix+"-"+policy+"-b"+str(bw)+".txt"
        last_small = min_ratio
        last_large = max_ratio
        for trial in range(0, max_trial):
            bandwidth.word_writer(input_file, 9, 4, qos_ratio)  # Set QoS ratio.
            cdin = Chdir(directory)
            os.system("nice time ../../real-time-scheduler ../../"+input_file
                      +" ../../"+network_file+" "+stability_file_prefix)
            # Read new ratio.
            stability_ratio = bandwidth.read_stability_ratio(stability_file)
            cdin = Chdir("../../")
            if stability_ratio < 1.5:
                last_small = qos_ratio
                qos_ratio = bandwidth.bisection_adjust(
                    last_large, qos_ratio, qos_step, min_ratio, max_ratio)
            else:
                last_large = qos_ratio
                qos_ratio = bandwidth.bisection_adjust(
                    last_small, qos_ratio, qos_step, min_ratio, max_ratio)
            print ("Iteration "+str(trial+1)+"/"+str(max_trial)+
                   " completed! (Total: "+to_percentage(calculate_progress(
                       bw, bw_begin, bw_end, bw_step, trial, max_trial))+")")

def to_percentage(a):
    return str(float(int(round(1000*a)))/10)+"%"

def calculate_progress(bw, bw_begin, bw_end, bw_step, trial, max_trial):
    major_progress = ( ( (float(bw)-float(bw_begin)) / float(bw_step) ) /
                       ( (float(bw_end)-float(bw_begin)) / float(bw_step)+1 ) )
    minor_progress = ( float(trial) / float(max_trial) /
                       ( (float(bw_end)-float(bw_begin)) / float(bw_step)+1 ) )
    return major_progress+minor_progress

if __name__ == "__main__":
    main()
