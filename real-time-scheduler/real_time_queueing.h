//
//  real_time_queueing.h
//  real-time-scheduler
//
//  Copyright (c) 2013-2014 Xiaohan Kang. All rights reserved.
//

#ifndef REAL_TIME_SCHEDULER_REAL_TIME_QUEUEING_H_
#define REAL_TIME_SCHEDULER_REAL_TIME_QUEUEING_H_

#include <random>
#include <vector>
#include <string>
#include "./policy.h"
#include "./common.h"

class Packet {  // packet with birth time and deadline
public:  // NOLINT
    Packet(int i = -1, int j = -1) {birth_time_ = i; deadline_ = j;}
        // TODO(Veggente): initializer list
    int birth_time() const {return birth_time_;}
    int deadline() const {return deadline_;}
    int delay_bound() const {return deadline_-birth_time_+1;}
private:  // NOLINT
    int birth_time_;  // birth time
    int deadline_;  // deadline
};

class QueueingSystem {
public:  // NOLINT
         // TODO(Veggente): unnecessary methods cleanup
    QueueingSystem(const BooleanMatrix &m,
                   Policy s, Ratios q, int b,
                   int d, const std::string &f, int n);
    Queues per_link_queue() const {return per_link_queue_;}
    Counters per_link_deficit() const {return per_link_deficit_;}
    int network_size() const {return network_size_;}
    Policy scheduler() const {return scheduler_;}
    Ratios qos() const {return qos_;}
    int bandwidth() const {return bandwidth_;}
    TieBreaker intra_link_tie_breaker() const {return intra_link_tie_breaker_;}
    BooleanMatrix maximal_schedule_matrix() const {
        return maximal_schedule_matrix_;}
    int system_clock() const {return system_clock_;}
    void clock_tick();
    int max_delay_bound() const {return max_delay_bound_;}
    std::string output_filename() const {return output_filename_;}
    void arrive(const Traffic &traffic, std::mt19937 &rng);
    Counters queue_lengths();
    void depart(std::mt19937 &rng);  // NOLINT
    void output_deficits(const std::string &filename);
    int quarter_point();  // quarter point integer among num_iterations
    int half_point();  // half point integer among num_iterations
    void update_stability_counter();
    double lower_deficit_sum() const {return lower_deficit_sum_;}
    double upper_deficit_sum() const {return upper_deficit_sum_;}
    double stability_ratio();
    long sum_cumulative_arrival();
    long sum_cumulative_throughput();
private:  // NOLINT
    Queues per_link_queue_;
    Counters per_link_deficit_;
    int network_size_;
    Policy scheduler_;
    Ratios qos_;
    int bandwidth_;
    TieBreaker intra_link_tie_breaker_;
    BooleanMatrix maximal_schedule_matrix_;
    int system_clock_;
    int max_delay_bound_;
    std::string output_filename_;
    double lower_deficit_sum_;  // quarter-to-half deficit sum
                                // TODO(Veggente): use long instead of double
    double upper_deficit_sum_;  // half-to-whole deficit sum
                                // TODO(Veggente): use long instead of double
    int num_iterations_;
    Counters per_link_cumulative_throughput_;
    Counters per_link_cumulative_arrival_;
};

#endif  // REAL_TIME_SCHEDULER_REAL_TIME_QUEUEING_H_
