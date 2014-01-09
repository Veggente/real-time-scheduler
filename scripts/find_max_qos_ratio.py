#! /usr/bin/python
import bandwidth
import os

# parameters to vary
qos_ratio = 0.5 # initial ratio
max_trial = 20 # maximum number of trials before terminating
qos_step = 0.001 # step size
policy_indicator = 4
bw = 4 # bandwidth
num_iterations = 1000 # number of iterations
save_config_and_deficit = 0 # save data or not

# parameters not likely to vary
min_ratio = 0.0
max_ratio = 1.0
input_file = "input.txt"
network_file = "network-5.txt"
stability_file_prefix = "stability"

# initialization
bandwidth.word_writer(input_file, 11, 3, policy_indicator) # set policy indicator
policy = ""
if policy_indicator == 1:
    policy = "ldf"
elif policy_indicator == 2:
    policy = "edf"
elif policy_indicator == 4:
    policy = "sdbf" # other policies are ignored
bandwidth.word_writer(input_file, 7, 2, bw) # set bandwidth
bandwidth.word_writer(input_file, 12, 3, 1) # set time seed
bandwidth.word_writer(input_file, 13, 5, save_config_and_deficit)
bandwidth.word_writer(input_file, 10, 4, num_iterations) # set number of iterations
stability_file = stability_file_prefix+"-"+policy+"-b"+str(bw)+".txt"
last_small = min_ratio
last_large = max_ratio
for trial in range(0, max_trial):
    bandwidth.word_writer(input_file, 9, 4, qos_ratio) # set qos ratio
    os.system("nice time ./real-time-scheduler "+input_file+" "+network_file+" "+stability_file_prefix)
    # read new ratio
    stability_ratio = bandwidth.read_stability_ratio(stability_file)
        # if stable, go up
    if stability_ratio < 1.5:
        last_small = qos_ratio
        qos_ratio = bandwidth.bisection_adjust(last_large, qos_ratio, qos_step, min_ratio, max_ratio)
        # if not, go down
    else:
        last_large = qos_ratio
        qos_ratio = bandwidth.bisection_adjust(last_small, qos_ratio, qos_step, min_ratio, max_ratio)
    # if ratio has converged, stop
    print "Iteration "+str(trial+1)+"/"+str(max_trial)+" completed!"
