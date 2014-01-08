#! /usr/bin/env python2.7
def change_qos_ratio( new_ratio ):
    import os
    input_file = "input.txt"
    temp_file = "input_temp.txt"
    f = open(input_file)
    new_file = open(temp_file, 'w')
    for i, line in enumerate(f):
        if i == 8:
            new_line = line.split()
            new_line[3] = repr(new_ratio)+"\n"
            line = " ".join(new_line)
        new_file.write(line)
    f.close()
    os.remove(input_file)
    os.rename(temp_file, input_file)

def run_real_time_scheduler():
    import subprocess
    args = "nice time ./real-time-scheduler input.txt network-5.txt stability".split()
    popen = subprocess.Popen(args, stdout=subprocess.PIPE)
    popen.wait()
    output = popen.stdout.read()
    print output

def read_stability_ratio(input_file):
    import os
    f = open(input_file)
    new_ratio = float(f.read().split()[-1])
    f.close()
    return new_ratio
