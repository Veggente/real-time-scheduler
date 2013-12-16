//
//  simulator.h
//  real-time-scheduler
//
//  Copyright (c) 2013 Xiaohan Kang. All rights reserved.
//

#ifndef REAL_TIME_SCHEDULER_SIMULATOR_H_
#define REAL_TIME_SCHEDULER_SIMULATOR_H_

#include <string>
#include <random>
#include "./common.h"

int simulator(const std::string &input_file);

class Simulator {
public:
    Simulator();
    void load_config(const std::string &config_filename);
    void save_config();
    void arrive(std::mt19937 rng);
    void depart(std::mt19937 rng);
    void save_deficits();
    void clock_tick();
    void progress_bar();
private:
    NetworkType network_type_;
    int network_size_;
    int interference_radius_;  // of the conflict graph
    BooleanMatrix maximal_schedule_matrix_;
    ArrivalDistribution arrival_dist_;
    IntegerVector min_packet_;  // only for uniform arrival
    IntegerVector max_packet_;
    Ratios binom_param_;  // only for binomial arrival
    IntegerVector min_delay_bound_;
    IntegerVector max_delay_bound_;
    IntegerVector bandwidth_;
    Ratios base_qos_;
    Ratios ratio_;
    int num_iterations_;
    QueueingSystem3D queueing_system_;
    String3D output_filename_;
};

#endif  // REAL_TIME_SCHEDULER_SIMULATOR_H_
