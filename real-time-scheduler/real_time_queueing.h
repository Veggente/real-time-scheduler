//
//  real_time_queueing.h
//  real-time-scheduler
//
//  Copyright (c) 2013 Xiaohan Kang. All rights reserved.
//

#ifndef REAL_TIME_SCHEDULER_REAL_TIME_QUEUEING_H_
#define REAL_TIME_SCHEDULER_REAL_TIME_QUEUEING_H_

#include <vector>
#include <random>
#include "./policy.h"

class Packet;
typedef std::vector<Packet> PacketSet;
typedef std::vector<PacketSet> Traffic;
typedef std::vector<PacketSet> Queues;
typedef std::vector<int> Counters;
typedef std::vector<double> Ratios;

class Packet {  // packet with birth time and deadline
public:  // NOLINT
    Packet(int i = -1, int j = -1) {birth_time_ = i; deadline_ = j;}
    int birth_time() const {return birth_time_;}
    int deadline() const {return deadline_;}
    int delay_bound() const {return deadline_-birth_time_+1;}
private:  // NOLINT
    int birth_time_;  // birth time
    int deadline_;  // deadline
};

class QueueingSystem {
public:  // NOLINT
    QueueingSystem(int i, Policy s, Ratios q);
    Queues per_link_queue() const {return per_link_queue_;}
    Counters per_link_deficit() const {return per_link_deficit_;}
    int network_size() const {return network_size_;}
    Policy scheduler() const {return scheduler_;}
    Ratios qos() const {return qos_;}
    void arrive(const Traffic &traffic, std::mt19937 &rng);
        // TODO(Veggente): depart
private:  // NOLINT
    Queues per_link_queue_;
    Counters per_link_deficit_;
    int network_size_;
    Policy scheduler_;
    Ratios qos_;
};

#endif  // REAL_TIME_SCHEDULER_REAL_TIME_QUEUEING_H_