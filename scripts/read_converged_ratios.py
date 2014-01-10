#! /usr/bin/python
import bandwidth
import sys, getopt

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "p:x:y:d:b:e:s:")
    except getopt.GetoptError as err:
        # print help information and exit:
        print str(err) # will print something like "option -a not recognized"
        sys.exit(2)
    policy = "ldf"
    x = "1"
    y = "1" # y = sys.argv[3]
    delay_bound = "1" # sys.argv[4]
    bw_start = 1 # int(float(sys.argv[5]))
    bw_end = 1 # int(float(sys.argv[6]))
    bw_step = 1 # int(float(sys.argv[7]))
    for o, a in opts:
        if o == "-p":
            policy = a
        elif o == "-x":
            x = a
        elif o == "-y":
            y = a
        elif o == "-d":
            delay_bound = a
        elif o == "-b":
            bw_start = int(float(a))
        elif o == "-e":
            bw_end = int(float(a))
        elif o == "-s":
            bw_step = int(float(a))
        else:
            assert False, "unhandled option"
    print "The critical QoS ratios for policy "+policy+", base QoS ("+x+", "+y+"), delay bound "+delay_bound+", bandwidths "+str(bw_start)+":"+str(bw_step)+":"+str(bw_end)+" are"
    for bw in bandwidth.my_range(bw_start, bw_end, bw_step):
        stability_file = "./qos-x"+str(x)+"-y"+str(y)+"/delay_bound_"+delay_bound+"/stability-"+policy+"-b"+str(bw)+".txt"
        f = open(stability_file)
        print f.read().split()[-2]
        f.close()
    print "The associated stability ratios are"
    for bw in bandwidth.my_range(bw_start, bw_end, bw_step):
        stability_file = "./qos-x"+str(x)+"-y"+str(y)+"/delay_bound_"+delay_bound+"/stability-"+policy+"-b"+str(bw)+".txt"
        f = open(stability_file)
        print f.read().split()[-1]
        f.close()
if __name__ == "__main__":
    main()
