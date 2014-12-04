#!/usr/bin/python
import os
import sys

def main():
    qos_x = [0, 0.16, 0.32, 0.51, 0.73, 1, 1, 1, 1, 1, 1]
    qos_y = [1, 1, 1, 1, 1, 1, 0.73, 0.51, 0.32, 0.16, 0]
    qos_len = len(qos_x)
    delays = [1, 2]
    policy = sys.argv[1]
    bw_begin = 24
    bw_end = 24
    bw_step = 24
    for delay in delays:
        for qos_it in range(qos_len):
            os.system("./find_critical_qos.py -p "+policy+" -x "+str(qos_x[qos_it])+" -y "+str(qos_y[qos_it])+" -d "+str(delay)+" -b "+str(bw_begin)+" -e "+str(bw_end)+" -s "+str(bw_step))
            print "Done with QoS ("+str(qos_x[qos_it])+", "+str(qos_y[qos_it])+"), delay", delay

if __name__ == "__main__":
    main()
