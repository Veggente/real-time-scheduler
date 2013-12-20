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
                                 IntegerVector min_packet,
                                 IntegerVector max_packet,
                                 IntegerVector min_delay_bound,
                                 IntegerVector max_delay_bound,
                                 std::mt19937 &rng);  // NOLINT

Traffic generate_binomial_traffic(int network_size, int current_time,
                                  IntegerVector max_packet, Ratios binom_param,
                                  IntegerVector min_delay_bound,
                                  IntegerVector max_delay_bound,
                                  std::mt19937 &rng);  // NOLINT



#endif  // REAL_TIME_SCHEDULER_TRAFFIC_GENERATOR_H_
