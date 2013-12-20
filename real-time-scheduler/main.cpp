//
//  main.cpp
//  real-time-scheduler
//
//  Copyright (c) 2013 Xiaohan Kang. All rights reserved.
//

#include <random>
#include <string>
#include "./simulator.h"

int main(int argc, const char * argv[]) {
    std::string input_file(argv[1]);  // TODO(Veggente): check arguments
    Simulator simulator;
    simulator.init(input_file);
    simulator.save_config();
    std::mt19937 rng;
    for (int time_slot = 0; time_slot < simulator.num_iterations();
         ++time_slot) {
        simulator.arrive(rng);
        simulator.depart(rng);
        simulator.save_deficits();
        simulator.progress_bar();
        simulator.clock_tick();
    }
}
