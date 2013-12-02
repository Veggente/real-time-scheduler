//
//  real_time_queueing.cpp
//  real-time-scheduler
//
//  Copyright (c) 2013 Xiaohan Kang. All rights reserved.
//

#include "./real_time_queueing.h"
#include <iostream>  // NOLINT
#include <algorithm>

Counters deficit_arrival(const Traffic &traffic, const Ratios &qos,
                         std::mt19937 &rng);
bool cmp_delay_bound(const Packet &a, const Packet &b);
bool cmp_deadline(const Packet &a, const Packet &b);

QueueingSystem::QueueingSystem(const BooleanMatrix &m, Policy s, Ratios q,
                               int b) {
    maximal_schedule_matrix_ = m;
    network_size_ = static_cast<int>(m[0].size());
    per_link_deficit_.insert(per_link_deficit_.begin(), network_size_, 0);
    PacketSet empty_packet_set;
    per_link_queue_.insert(per_link_queue_.begin(), network_size_,
                           empty_packet_set);
    scheduler_ = s;
    qos_ = q;
    if (scheduler() == SDBF) {  // TODO(Veggente): customizable tie-breakers
        intra_link_tie_breaker_ = DELAY_BOUND;
    } else {
        intra_link_tie_breaker_ = DEADLINE;
    }
    bandwidth_ = b;
    system_clock_ = 0;
}

void QueueingSystem::arrive(const Traffic &traffic, std::mt19937 &rng) {
    // TODO(Veggente): first check compatibility
    Counters deficit_increase = deficit_arrival(traffic, qos(), rng);
    for (int i = 0; i < network_size(); ++i) {
        per_link_queue_[i].insert(per_link_queue_[i].begin(),
                                  traffic[i].begin(), traffic[i].end());
        if (intra_link_tie_breaker() == DELAY_BOUND) {
            make_heap(per_link_queue_[i].begin(), per_link_queue_[i].end(),
                      cmp_delay_bound);
        } else if (intra_link_tie_breaker() == DEADLINE) {
            make_heap(per_link_queue_[i].begin(), per_link_queue_[i].end(),
                      cmp_deadline);
        }  // for RANDOM no tie-breaker is specified
        per_link_deficit_[i] += deficit_increase[i];
    }
}

Counters QueueingSystem::queue_lengths() {
    Counters queues;
    for (int i = 0; i < network_size(); ++i) {
        queues.push_back(static_cast<int>(per_link_queue()[i].size()));
    }
    return queues;
}

void QueueingSystem::depart(std::mt19937 &rng) {  // NOLINT
    for (int sub_time_slot = 0; sub_time_slot < bandwidth(); ++sub_time_slot) {
        BooleanVector scheduled_links(network_size(), false);
        if (scheduler() == LDF) {
            scheduled_links = ldf(per_link_queue(), per_link_deficit(),
                                  maximal_schedule_matrix(), rng);
        } else {
            // TODO(Veggente): other policies
        }
        for (int i = 0; i < network_size(); ++i) {
            if (scheduled_links[i]) {
                if (intra_link_tie_breaker() == DEADLINE) {
                        // TODO(Veggente): customizable tie-breakers
                    pop_heap(per_link_queue_[i].begin(),
                             per_link_queue_[i].end(), cmp_deadline);
                } else {
                    pop_heap(per_link_queue_[i].begin(),
                             per_link_queue_[i].end(), cmp_delay_bound);
                }
                per_link_queue_[i].pop_back();
                if (per_link_deficit()[i] > 0) {
                    per_link_deficit_[i] -= 1;  // deficit decreases
                }
            }
        }
    }
}

Counters deficit_arrival(const Traffic &traffic, const Ratios &qos,
                         std::mt19937 &rng) {
    int network_size = static_cast<int>(traffic.size());
    Counters deficit_increase(network_size, 0);
    for (int i = 0; i < network_size; ++i) {
        std::binomial_distribution<> d(static_cast<int>(traffic[i].size()),
                                       qos[i]);
            // TODO(Veggente): share the distributions to reduce object
            // generation
        deficit_increase[i] = d(rng);
    }
    return deficit_increase;
}

bool cmp_delay_bound(const Packet &a, const Packet &b) {
    return (a.delay_bound() > b.delay_bound());
        // a is considered less than b when a has a larger delay bound
}

bool cmp_deadline(const Packet &a, const Packet &b) {
    return (a.deadline() > b.deadline());
        // a is considered less than b when a has a larger deadline
}
