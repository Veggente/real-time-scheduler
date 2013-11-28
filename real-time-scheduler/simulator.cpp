//
//  simulator.cpp
//  real-time-scheduler
//
//  Copyright (c) 2013 Xiaohan Kang. All rights reserved.
//

#include "./simulator.h"
#include <vector>
#include "./network_generator.h"

int simulator(int network_size, int num_iterations) {
    // generate the network
    BooleanMatrix maximal_schedule_matrix =
        gen_max_matrix_collocated(network_size);
    for (int time_slot = 0; time_slot < num_iterations; ++time_slot) {
        // TODO(Veggente): random arrivals
        // TODO(Veggente): schedulers
        // TODO(Veggente): outputs
    }
    return 0;
}
