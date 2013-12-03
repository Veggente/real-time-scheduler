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

QueueingSystem::QueueingSystem(const BooleanMatrix &m,
                               Policy s, Ratios q, int b,
                               int d) {
    maximal_schedule_matrix_ = m;
    scheduler_ = s;
    qos_ = q;
    bandwidth_ = b;
    system_clock_ = 0;
    max_delay_bound_ = d;
    network_size_ = static_cast<int>(maximal_schedule_matrix_[0].size());
    per_link_deficit_.insert(per_link_deficit_.begin(), network_size(), 0);
    PacketSet empty_packet_set;
    per_link_queue_.insert(per_link_queue_.begin(), network_size(),
                           empty_packet_set);
    if (scheduler() == SDBF) {  // TODO(Veggente): customizable tie-breakers
        intra_link_tie_breaker_ = DELAY_BOUND;
    } else {
        intra_link_tie_breaker_ = DEADLINE;
    }
}

void QueueingSystem::arrive(const Traffic &traffic, std::mt19937 &rng) {
    // TODO(Veggente): first check compatibility
    Counters deficit_increase = deficit_arrival(traffic, qos(), rng);
    for (int i = 0; i < network_size(); ++i) {
        per_link_queue_[i].insert(per_link_queue_[i].begin(),
                                  traffic[i].begin(), traffic[i].end());
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
    for (int i = 0; i < network_size(); ++i) {  // make heaps
        if (intra_link_tie_breaker() == DELAY_BOUND) {
            make_heap(per_link_queue_[i].begin(), per_link_queue_[i].end(),
                      cmp_delay_bound);
        } else if (intra_link_tie_breaker() == DEADLINE) {
            make_heap(per_link_queue_[i].begin(), per_link_queue_[i].end(),
                      cmp_deadline);
        }  // for RANDOM no tie-breaker is specified, so no heap is made
    }
    for (int sub_time_slot = 0; sub_time_slot < bandwidth(); ++sub_time_slot) {
        BooleanVector scheduled_links(network_size(), false);
        if (scheduler() == LDF) {
            scheduled_links = ldf(per_link_queue(), per_link_deficit(),
                                  maximal_schedule_matrix(), rng);
        } else if (scheduler() == EDF) {
            scheduled_links = edf(per_link_queue(), per_link_deficit(),
                                  maximal_schedule_matrix(), system_clock(),
                                  max_delay_bound(), rng);
        } else if (scheduler() == SDBF) {
            scheduled_links = sdbf(per_link_queue(), per_link_deficit(),
                                   maximal_schedule_matrix(), max_delay_bound(),
                                   rng);
        } else {
            // TODO(Veggente): should not happen
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

void QueueingSystem::clock_tick() {
    ++system_clock_;
    for (int i = 0; i < network_size(); ++i) {
        for (int j = static_cast<int>(per_link_queue()[i].size())-1; j >= 0;
             --j) {
            if (per_link_queue()[i][j].deadline() < system_clock()) {
                per_link_queue_[i].erase(per_link_queue_[i].begin()+j);
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
