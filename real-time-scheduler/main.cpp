//
//  main.cpp
//  real-time-scheduler
//
//  Copyright (c) 2013-2014 Xiaohan Kang. All rights reserved.
//

#include <random>
#include <string>
#include "./simulator.h"

int main(int argc, const char * argv[]) {
    std::string input_file(argv[1]);  // TODO(Veggente): check arguments
    std::string network_file(argv[2]);
    std::string stability_file(argv[3]);
    std::mt19937 rng;
    Simulator simulator;
    bool save_config_and_deficit = simulator.init(input_file, network_file,
                                                  rng);
    if (save_config_and_deficit) {
        simulator.save_config();
    }
    for (int time_slot = 0; time_slot < simulator.num_iterations();
         ++time_slot) {
        simulator.arrive(rng);
        simulator.depart(rng);
        simulator.update_stability_counter();
        if (save_config_and_deficit) {
            simulator.save_deficits();
        }
        simulator.progress_bar();
        simulator.clock_tick();
    }
    simulator.save_stability_ratios(stability_file);
}
