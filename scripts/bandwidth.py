#!/usr/bin/python
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
    args = "nice time ../real-time-scheduler input.txt network-5.txt stability".split()
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

def word_reader(filename, row, column, num_indicator):
    import os
    f = open(filename)
    return_string = "";
    for i, line in enumerate(f):
        if i == row-1:
            return_string = line.split()[column-1]
    f.close()
    if num_indicator:
        return float(return_string)
    else:
        return return_string

def word_writer(filename, row, column, new_word):
    import os
    temp_filename = filename+"_temp"
    f = open(filename)
    new_file = open(temp_filename, 'w')
    for i, line in enumerate(f):
        if i == row-1:
            new_line = line.split()
            new_line[column-1] = str(new_word)
            line = " ".join(new_line)
            new_file.write(line)
            new_file.write('\n')
        else:
            new_file.write(line)
    f.close()
    os.remove(filename)
    os.rename(temp_filename, filename)

def bisection_adjust(last_correct_value, this_value, step_size, min_value, max_value):
    import math
    import random
    if math.fabs(last_correct_value-this_value) < step_size:
        if random.random() > 0.5:
            return min(this_value+step_size, max_value)
        else:
            return max(this_value-step_size, min_value)
    elif last_correct_value > this_value:
        return round((last_correct_value-this_value)/2.0/step_size)*step_size+this_value
    else:
        return this_value-round((this_value-last_correct_value)/2.0/step_size)*step_size

def bisection_adjust_int(last_correct_value, this_value, step_size, min_value, max_value):
    import math
    import random
    if math.fabs(last_correct_value-this_value) <= step_size:
#        if random.random() > 0.5:
#            return min(this_value+step_size, max_value)
#        else:
#            return max(this_value-step_size, min_value)
        if last_correct_value > this_value:
            return this_value+step_size
        elif last_correct_value == this_value:
            if random.random() > 0.5:
                return this_value+step_size
        return this_value-step_size
    elif last_correct_value > this_value:
        return (last_correct_value-this_value)/2/step_size*step_size+this_value
    else:
        return this_value-(this_value-last_correct_value)/2/step_size*step_size

def my_range(start, end, step):
    while start <= end:
        yield start
        start += step
