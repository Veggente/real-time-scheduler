//
//  simulator.h
//  real-time-scheduler
//
//  Copyright (c) 2013 Xiaohan Kang. All rights reserved.
//

#ifndef REAL_TIME_SCHEDULER_SIMULATOR_H_
#define REAL_TIME_SCHEDULER_SIMULATOR_H_

#include <string>
#include "./common.h"

int simulator(int network_size, int num_iterations);  // obsolete

int simulator_collocated(int network_size, ArrivalDistribution arr_dist,
                         int max_packet, double dist_param, int min_delay_bound,
                         int max_delay_bound, const IntegerVector &bandwidths,
                         const Ratios &qos, int num_iterations);

ArrivalDistribution string_to_arr_dist(std::string s);

#endif  // REAL_TIME_SCHEDULER_SIMULATOR_H_
