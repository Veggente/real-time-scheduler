#! /usr/bin/env python2.7
import bandwidth

min_ratio = 0.0
max_ratio = 1.0
qos_ratio = 0.5 # initial ratio
max_trial = 3
qos_step = 0.01
stability_file = "stability-ldf-b8.txt"
for trial in range(0, max_trial):
    # run with qos_ratio
        # change input file according to qos_ratio
    bandwidth.change_qos_ratio(qos_ratio)
        # run real-time-scheduler
    bandwidth.run_real_time_scheduler()
    # read new ratio
    stability_ratio = bandwidth.read_stability_ratio(stability_file)
        # if stable, go up
    if stability_ratio < 1.5:
        qos_ratio += qos_step
        # if not, go down
    else:
        qos_ratio -= qos_step
    # if ratio has converged, stop
