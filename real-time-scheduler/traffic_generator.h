//
//  traffic_generator.h
//  real-time-scheduler
//
//  Copyright (c) 2013 Xiaohan Kang. All rights reserved.
//

#ifndef REAL_TIME_SCHEDULER_TRAFFIC_GENERATOR_H_
#define REAL_TIME_SCHEDULER_TRAFFIC_GENERATOR_H_

#include <random>
#include "./common.h"

Traffic generate_uniform_traffic(int network_size, int current_time,
                                 int min_packet, int max_packet,
                                 int min_delay_bound, int max_delay_bound,
                                 std::mt19937 &rng);

Traffic generate_binomial_traffic(int network_size, int current_time,
                                  int max_packet, double binom_param,
                                  int min_delay_bound, int max_delay_bound,
                                  std::mt19937 &rng);

#endif  // REAL_TIME_SCHEDULER_TRAFFIC_GENERATOR_H_
