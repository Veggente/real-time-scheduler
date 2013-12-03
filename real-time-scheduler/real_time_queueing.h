//
//  real_time_queueing.h
//  real-time-scheduler
//
//  Copyright (c) 2013 Xiaohan Kang. All rights reserved.
//

#ifndef REAL_TIME_SCHEDULER_REAL_TIME_QUEUEING_H_
#define REAL_TIME_SCHEDULER_REAL_TIME_QUEUEING_H_

#include <random>
#include <vector>
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
    QueueingSystem(const BooleanMatrix &maximal_schedule_matrix_,
                   Policy scheduler_, Ratios qos_, int bandwidth_,
                   int max_delay_bound_);
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
    void arrive(const Traffic &traffic, std::mt19937 &rng);
    Counters queue_lengths();
    void depart(std::mt19937 &rng);  // NOLINT
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
};

#endif  // REAL_TIME_SCHEDULER_REAL_TIME_QUEUEING_H_
