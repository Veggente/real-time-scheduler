#! /usr/bin/python
import bandwidth
import sys

policy = sys.argv[1]
x = sys.argv[2]
y = sys.argv[3]
delay_bound = sys.argv[4]
bw_start = int(float(sys.argv[5]))
bw_end = int(float(sys.argv[6]))
bw_step = int(float(sys.argv[7]))
print "The critical QoS ratios for policy "+policy+", base QoS ("+x+", "+y+"), delay bound "+delay_bound+", bandwidths "+str(bw_start)+":"+str(bw_step)+":"+str(bw_end)+" are"
for bw in bandwidth.my_range(bw_start, bw_end, bw_step):
    stability_file = "./qos-x"+str(x)+"-y"+str(y)+"/delay_bound_"+str(delay_bound)+"/stability-"+policy+"-b"+str(bw)+".txt"
    f = open(stability_file)
    print f.read().split()[-2]
    f.close()
print "The associated stability ratios are"
for bw in bandwidth.my_range(bw_start, bw_end, bw_step):
    stability_file = "./qos-x"+str(x)+"-y"+str(y)+"/delay_bound_"+str(delay_bound)+"/stability-"+policy+"-b"+str(bw)+".txt"
    f = open(stability_file)
    print f.read().split()[-1]
    f.close()
