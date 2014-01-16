//
//  policy.cpp
//  real-time-scheduler
//
//  Copyright (c) 2013-2014 Xiaohan Kang. All rights reserved.
//

#include "./policy.h"
#include <algorithm>
#include "./common.h"
#include "./real_time_queueing.h"
    // TODO(Veggente): why do we need to include this?

bool comp_pairs(const IndexPair &p1, const IndexPair &p2);
BooleanVector available_queues(const Queues &q);

BooleanVector greedy(const BooleanVector &availability,
                     const Counters &priority,
                     const BooleanMatrix &maximal_schedule_matrix,
                         // must be maximal
                     std::mt19937 &rng) {
    BooleanVector schedule;
    int network_size = static_cast<int>(availability.size());
    long min_priority = *std::min_element(priority.begin(), priority.end());
    Counters available_priority;  // actual priority
    IndexMapping available_priority_map;  // map of indices and priority
    for (int i = 0; i < network_size; ++i) {
        if (availability[i]) {
            available_priority.push_back(priority[i]);
        } else {
            available_priority.push_back(min_priority-1);
                // give unavailable links lowest priority
        }
        IndexPair p(available_priority[i], i);  // make map
        available_priority_map.push_back(p);
    }
    // Sort the pairs with random tie-breaking
    std::shuffle(available_priority_map.begin(), available_priority_map.end(),
                 rng);  // shuffle the pairs in the map
    std::stable_sort(available_priority_map.begin(),
                     available_priority_map.end(), comp_pairs);
        // stable_sort the pairs according to the priority

    // Find the suitable maximal schedule starting from the pair with highest
    // priority
    IndexMapping::const_reverse_iterator rit;
    // Start from high priority
    IntegerVector survivor;  // indices of surviving maximal schedules
    IntegerVector new_survivor;  // temp for swapping
    for (int i = 0; i < maximal_schedule_matrix.size(); ++i) {
            // indices are initialized 0 up to size of maximal_schedule_matrix
        survivor.push_back(i);
    }
    for (rit = available_priority_map.rbegin();
         rit < available_priority_map.rend(); ++rit) {
        if (survivor.size() == 1) {  // unique maximal schedule
            break;
        } else {
            new_survivor.clear();
            new_survivor.assign(survivor.begin(), survivor.end());
            // copy survivor to new_survivor
            IntegerVector::iterator it = new_survivor.begin();
            while (it != new_survivor.end()) {  // find compatible schedules
                if ( !( (maximal_schedule_matrix[*it])[rit->second] ) ) {
                    new_survivor.erase(it);
                        // new_survivor.size() decreases by 1
                } else {
                    ++it;
                }
            }
            if (new_survivor.size() > 0) {  // if new_survivor is not empty,
                                           // updated it to survivor
                survivor.swap(new_survivor);
            }  // otherwise disregard new_survivor
            new_survivor.clear();
        }
    }
    for (int i = 0; i < network_size; ++i) {
        if (availability[i]) {
            schedule.push_back(maximal_schedule_matrix[survivor.front()][i]);
        } else {
            schedule.push_back(false);  // cannot schedule unavailable links
        }
    }
    return schedule;
}

BooleanVector ldf(const Queues &queues_deadline_heap,
                  const Counters &deficits,
                  const BooleanMatrix &maximal_schedule_matrix,
                  std::mt19937 &rng) {
    return greedy(available_queues(queues_deadline_heap), deficits,
                  maximal_schedule_matrix, rng);
}

BooleanVector edf(const Queues &queues_deadline_heap,
                  const Counters &deficits,
                  const BooleanMatrix &maximal_schedule_matrix,
                  int current_time, int max_delay_bound, std::mt19937 &rng) {
    Counters priority;
    int network_size = static_cast<int>(queues_deadline_heap.size());
    for (int i = 0; i < network_size; ++i) {
        if (queues_deadline_heap[i].empty()) {
            priority.push_back(0);
                // priority does not matter for unavailable links
        } else {
            int remaining_delay_bound = queues_deadline_heap[i][0].deadline()
                                        -current_time+1;
            if (deficits[i] > 0) {
                priority.push_back(-remaining_delay_bound);
            } else {  // zero deficit links have lower priority
                priority.push_back(-remaining_delay_bound-max_delay_bound);
            }
        }
    }
    return greedy(available_queues(queues_deadline_heap), priority,
                  maximal_schedule_matrix, rng);
}

BooleanVector sdbf(const Queues &queues_delay_bound_heap,  // min delay bound
                                                           // heap
                   const Counters &deficits,
                   const BooleanMatrix &maximal_schedule_matrix,
                   int max_delay_bound, std::mt19937 &rng) {
    Counters priority;
    int network_size = static_cast<int>(queues_delay_bound_heap.size());
    for (int i = 0; i < network_size; ++i) {
        if (queues_delay_bound_heap[i].empty()) {
            priority.push_back(0);
            // priority does not matter for unavailable links
        } else {
            if (deficits[i] > 0) {
                priority.push_back(
                    -queues_delay_bound_heap[i][0].delay_bound());
            } else {  // zero deficit links have lower priority
                priority.push_back(-queues_delay_bound_heap[i][0].delay_bound()
                                   -max_delay_bound);
            }
        }
    }
    return greedy(available_queues(queues_delay_bound_heap), priority,
                  maximal_schedule_matrix, rng);
}

BooleanVector edf_naive(const Queues &queues_deadline_heap,
                        const BooleanMatrix &maximal_schedule_matrix,
                        int current_time, std::mt19937 &rng) {
    int network_size = static_cast<int>(queues_deadline_heap.size());
    Counters all_one_deficits(network_size, 1);
    int zero_max_delay_bound = 0;
    return edf(queues_deadline_heap, all_one_deficits,
               maximal_schedule_matrix, current_time,
               zero_max_delay_bound, rng);
}

BooleanVector sdbf_naive(const Queues &queues_deadline_heap,
                         const BooleanMatrix &maximal_schedule_matrix,
                         std::mt19937 &rng) {
    int network_size = static_cast<int>(queues_deadline_heap.size());
    Counters all_one_deficits(network_size, 1);
    int zero_max_delay_bound = 0;
    return sdbf(queues_deadline_heap, all_one_deficits, maximal_schedule_matrix,
                zero_max_delay_bound, rng);
}

BooleanVector maximal(const Queues &queues_deadline_heap, const BooleanMatrix &maximal_schedule_matrix, std::mt19937 &rng) {
    int network_size = static_cast<int>(queues_deadline_heap.size());
    Counters all_zero_priority(network_size, 0);
    return greedy(available_queues(queues_deadline_heap), all_zero_priority, maximal_schedule_matrix, rng);
}

bool comp_pairs(const IndexPair &p1, const IndexPair &p2) {
        // for random tie-breaking in greedy()
    return (p1.first < p2.first);
}

BooleanVector available_queues(const Queues &q) {
    BooleanVector availability;
    for (int i = 0; i < q.size(); ++i) {
        if (q[i].empty()) {
            availability.push_back(false);
        } else {
            availability.push_back(true);
        }
    }
    return availability;
}
