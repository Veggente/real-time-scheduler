#! /usr/bin/python
import bandwidth
policy = "ldf"
for bw in bandwidth.my_range(8, 80, 8):
    stability_file = "stability-"+policy+"-b"+str(bw)+".txt"
    f = open(stability_file)
    print f.read().split()[-2]
