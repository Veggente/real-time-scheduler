#! /usr/bin/python
import bandwidth
import sys, getopt

def main(argv):
    try:
        opts, args = getopt.getopt(argv, "bp:d:")
    except getopt.GetoptError as err:
        # print help information and exit:
        print str(err) # will print something like "option -a not recognized"
        sys.exit(2)
    policy = "ldf"
#    x = "1"
#    y = "1" # y = sys.argv[3]
    delay_bound = "1" # sys.argv[4]
    bw_start = 1 # int(float(sys.argv[5]))
    bw_end = 1 # int(float(sys.argv[6]))
    bw_step = 1 # int(float(sys.argv[7]))
    qos_bandwidth_plot = False # default is QoS stability region plot
    for o, a in opts:
        if o == "-p":
            policy = a
#        elif o == "-x":
#            x = a
#        elif o == "-y":
#            y = a
        elif o == "-d":
            delay_bound = a
#        elif o == "-b":
#            bw_start = int(float(a))
#        elif o == "-e":
#            bw_end = int(float(a))
#        elif o == "-s":
#            bw_step = int(float(a))
        elif o == "-b":
            qos_bandwidth_plot = True # QoS bandwidth function plot
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
    if qos_bandwidth_plot:
        x_list = [1]
        y_list = [1]
        bw_start = 8
        bw_end = 80
        bw_step = 8
#    print '; '.join(map(str, read_policy_for_all_directions(policy, x_list, y_list, delay_bound, bw_start, bw_end, bw_step)))
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

if __name__ == "__main__":
    main(sys.argv[1:])
