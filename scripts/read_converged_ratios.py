#!/usr/bin/python
import bandwidth
import sys, getopt

def main(argv):
    try:
        opts, args = getopt.getopt(argv, "m:p:d:x:y:")
    except getopt.GetoptError as err:
        # print help information and exit:
        print str(err) # will print something like "option -a not recognized"
        sys.exit(2)
    policy = "ldf"
    x = "1"
    y = "1"
    delay_bound = "1" # sys.argv[4]
    bw_start = 1 # int(float(sys.argv[5]))
    bw_end = 1 # int(float(sys.argv[6]))
    bw_step = 1 # int(float(sys.argv[7]))
    mode = 0 # default: QoS stability region data
#    qos_bandwidth_plot = False # default is QoS stability region plot
    for o, a in opts:
        if o == "-p":
            policy = a
        elif o == "-x":
            x = a
        elif o == "-y":
            y = a
        elif o == "-d":
            delay_bound = a
#        elif o == "-b":
#            bw_start = int(float(a))
#        elif o == "-e":
#            bw_end = int(float(a))
#        elif o == "-s":
#            bw_step = int(float(a))
        elif o == "-m":
#            qos_bandwidth_plot = True # QoS bandwidth function plot
            mode = int(a) # 0(default): QoS stability region data
                          # 1: QoS--bandwidth data
                          # 2: critical bandwidth data for varying arrial
                          # 3: throughput
        else:
            assert False, "unhandled option"
#    print "The critical QoS ratios for policy "+policy+", base QoS ("+x+", "+y+"), delay bound "+delay_bound+", bandwidths "+str(bw_start)+":"+str(bw_step)+":"+str(bw_end)+" are"
#    print read_policy(policy, x, y, delay_bound, bw_start, bw_end, bw_step)
#    for bw in bandwidth.my_range(bw_start, bw_end, bw_step):
#        stability_file = "./qos-x"+str(x)+"-y"+str(y)+"/delay_bound_"+delay_bound+"/stability-"+policy+"-b"+str(bw)+".txt"
#        f = open(stability_file)
#        print f.read().split()[-2]
#        f.close()
#    print "The associated stability ratios are"
#    for bw in bandwidth.my_range(bw_start, bw_end, bw_step):
#        stability_file = "./qos-x"+str(x)+"-y"+str(y)+"/delay_bound_"+delay_bound+"/stability-"+policy+"-b"+str(bw)+".txt"
#        f = open(stability_file)
#        print f.read().split()[-1]
#        f.close()
    x_list = [0, 0.16, 0.32, 0.51, 0.73, 1, 1, 1, 1, 1, 1]
    y_list = [1, 1, 1, 1, 1, 1, 0.73, 0.51, 0.32, 0.16, 0]
    bw_start = 24
    bw_end = 72
    bw_step = 24
    if mode == 3: # throughput
        bandwidths = bandwidth.my_range(30, 80, 5)
        throughput = [read_throughput(delay_bound, policy, bw) for bw in bandwidths]
        print throughput
    elif mode == 2: # critical bandwidth data
        max_arrivals = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        count = 10
        critical_bandwidths = [read_critical_bandwidth(x, y, delay_bound, policy, max_arrival, count) for max_arrival in max_arrivals]
        print critical_bandwidths
    else:
        if mode == 1:
            x_list = [1]
            y_list = [1]
            bw_start = 8
            bw_end = 80
            bw_step = 8
        print_list_on_different_lines(read_policy_for_all_directions(policy, x_list, y_list, delay_bound, bw_start, bw_end, bw_step))

def read_policy(policy, x, y, delay_bound, bw_start, bw_end, bw_step):
    a_list = []
    for bw in bandwidth.my_range(bw_start, bw_end, bw_step):
        stability_file = "./qos-x"+str(x)+"-y"+str(y)+"/delay_bound_"+delay_bound+"/stability-"+policy+"-b"+str(bw)+".txt"
        f = open(stability_file)
        a_list.append(float(f.read().split()[-2]))
        f.close()
    return a_list

def read_policy_for_all_directions(policy, x_list, y_list, delay_bound, bw_start, bw_end, bw_step):
    a_double_list = []
    for dir_it in range(0, len(x_list)):
        a_double_list.append(read_policy(policy, x_list[dir_it], y_list[dir_it], delay_bound, bw_start, bw_end, bw_step))
    return a_double_list

def print_list_on_different_lines(a_list):
    for element in a_list:
        print_list_on_single_line(element)

def print_list_on_single_line(a_list):
    prints = []
    for element in a_list:
        prints.append(element)
    print prints

def read_critical_bandwidth(x, y, delay_bound, policy, max_arrival, count):
    filename = "vary_arrival/qos-x"+str(x)+"-y"+str(y)+"/delay_bound_"+str(delay_bound)+"/stability-"+policy+"-m"+str(max_arrival)+".txt"
    words = []
    with open(filename, 'r') as f:
        for line in f:
            words.extend(line.split())
    words.reverse()
    converged_numbers = [int(word) for word in words[1::2][:count]] # read every other number backward from penultimate, counting up to count
    converged_ratios = [float(word) for word in words[0::2][:count]]
    # find the most frequent and the second most frequent
    winner = find_most_frequent_item(converged_numbers)
    runner_up = find_second_most_frequent_item(converged_numbers, winner)
    if winner == runner_up+1:
        if (read_stability(winner, converged_numbers, converged_ratios) and not read_stability(runner_up, converged_numbers, converged_ratios)):
            return winner
        # check convergence condition
    elif winner == runner_up-1:
        if (read_stability(runner_up, converged_numbers, converged_ratios) and not read_stability(winner, converged_numbers, converged_ratios)):
            return runner_up
        # check convergence condition
    return -1 # report indefinite state

def find_most_frequent_item(items):
    from collections import Counter
    import random
    lst = Counter(items).most_common()
    highest_count = max(i[1] for i in lst)
    values = [i[0] for i in lst if i[1] == highest_count]
    random.shuffle(values)
    return values[0]

def find_second_most_frequent_item(items, most_frequent_item):
    losers = [item for item in items if item != most_frequent_item]
    return find_most_frequent_item(losers)

def read_stability(bw_target, bw_list, ratio_list):
    stability_edge = 0
    stability_indicator = False
    for index, bw in enumerate(bw_list):
        if bw == bw_target:
            if ratio_list[index] < 1.5:
                stability_edge += 1
            else:
                stability_edge -= 1
    if stability_edge > 0:
        stability_indicator = True
    return stability_indicator

def read_throughput(delay, policy, bw):
    filename = "throughput/delay"+str(delay)+"/stability-"+policy+"-b"+str(bw)+".txt"
    words = []
    with open(filename, 'r') as f:
        for line in f:
            words.extend(line.split())
    num_arrival = float(words[-2])
    num_departure = float(words[-1])
    return num_departure/num_arrival

if __name__ == "__main__":
    main(sys.argv[1:])
