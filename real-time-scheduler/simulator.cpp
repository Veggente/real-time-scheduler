//
//  simulator.cpp
//  real-time-scheduler
//
//  Copyright (c) 2013 Xiaohan Kang. All rights reserved.
//

#include "./simulator.h"
#include <random>
#include "./network_generator.h"
#include "./traffic_generator.h"
#include "./real_time_queueing.h"

int simulator(int network_size, int num_iterations) {
    BooleanMatrix maximal_schedule_matrix =
        gen_max_matrix_collocated(network_size);
    int min_packet = 0;
    int max_packet = 2;
    int min_delay_bound = 1;
    int max_delay_bound = 3;
    Ratios qos(network_size, 0.6);
    int bandwidth = 2;
    std::mt19937 rng;
    QueueingSystem system_ldf(maximal_schedule_matrix, LDF, qos, bandwidth,
                              max_delay_bound);
    for (int time_slot = 0; time_slot < num_iterations; ++time_slot) {
        Traffic traffic = generate_uniform_traffic(network_size, time_slot,
                                                   min_packet, max_packet,
                                                   min_delay_bound,
                                                   max_delay_bound, rng);
        system_ldf.arrive(traffic, rng);
        system_ldf.depart(rng);
        // TODO(Veggente): outputs
        system_ldf.clock_tick();
    }
    return 0;
}
