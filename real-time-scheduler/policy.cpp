//
//  policy.cpp
//  real-time-scheduler
//
//  Copyright (c) 2013-2014 Xiaohan Kang. All rights reserved.
//

#include "./policy.h"
#include <algorithm>
#include <set>
#include "./common.h"
#include "./real_time_queueing.h"

bool comp_pairs(const IndexPair &p1, const IndexPair &p2);
BooleanVector available_queues(const Queues &q);
IntegerVector available_queue_set(const Queues &q);
BooleanMatrix restrict_schedule(const BooleanMatrix &maximal_schedule_matrix,
                                const IntegerVector &available_links);
IntegerVector get_indices(const BooleanMatrix &candidate);
IntegerMatrix traverse(const IntegerVector &available_links);
int64_t weighted_sum(const Counters &weight, const IntegerVector &link_set);
IntegerVector match(const IntegerVector &trial, const BooleanMatrix &candidate,
                    const IntegerVector &survivor);
void remove_elements(IntegerVector &original_set,  // NOLINT
                     const IntegerVector &elements);

BooleanVector greedy(const BooleanVector &availability,
                     const Counters &priority,
                     const BooleanMatrix &maximal_schedule_matrix,
                         // Need to be maximal.
                     std::mt19937 &rng) {
    BooleanVector schedule;
    int network_size = static_cast<int>(availability.size());
    int64_t min_priority = *std::min_element(priority.begin(), priority.end());
    Counters available_priority;  // Actual priority.
    IndexMapping available_priority_map;  // Map of indices and priority.
    for (int i = 0; i < network_size; ++i) {
        if (availability[i]) {
            available_priority.push_back(priority[i]);
        } else {
            available_priority.push_back(min_priority-1);
                // Give unavailable links lowest priority.
        }
        IndexPair p(available_priority[i], i);  // Make map.
        available_priority_map.push_back(p);
    }
    // Sort the pairs with random tie-breaking.
    std::shuffle(available_priority_map.begin(), available_priority_map.end(),
                 rng);  // Shuffle the pairs in the map.
    // stable_sort the pairs according to the priority in ascending order.
    std::stable_sort(available_priority_map.begin(),
                     available_priority_map.end(), comp_pairs);

    // Find the suitable maximal schedule starting from the pair with highest
    // priority.
    IndexMapping::const_reverse_iterator rit;
    // Start from high priority.
    IntegerVector survivor;  // Indices of surviving maximal schedules.
    IntegerVector new_survivor;  // Temporary for swapping.
    for (int i = 0; i < maximal_schedule_matrix.size(); ++i) {
        survivor.push_back(i);
    }
    for (rit = available_priority_map.rbegin();
         rit < available_priority_map.rend(); ++rit) {
        if (survivor.size() == 1) {  // Unique maximal schedule.
            break;
        } else {
            new_survivor.clear();
            new_survivor.assign(survivor.begin(), survivor.end());
            // Copy survivor to new_survivor.
            IntegerVector::iterator it = new_survivor.begin();
            while (it != new_survivor.end()) {  // Find compatible schedules.
                if ( !( (maximal_schedule_matrix[*it])[rit->second] ) ) {
                    new_survivor.erase(it);
                        // new_survivor.size() decreases by 1.
                } else {
                    ++it;
                }
            }
            if (new_survivor.size() > 0) {  // If new_survivor is not empty,
                                            // updated it to survivor.
                survivor.swap(new_survivor);
            }  // Otherwise disregard new_survivor.
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

BooleanVector maximal(const Queues &queues_deadline_heap,
                      const BooleanMatrix &maximal_schedule_matrix,
                      std::mt19937 &rng) {
    int network_size = static_cast<int>(queues_deadline_heap.size());
    Counters all_zero_priority(network_size, 0);
    return greedy(available_queues(queues_deadline_heap), all_zero_priority,
                  maximal_schedule_matrix, rng);
}

BooleanVector ldf_threshold(const Queues &queues_deadline_heap,
                            const Counters &deficits,
                            const BooleanMatrix &maximal_schedule_matrix,
                            int current_time, int max_delay_bound,
                            double threshold_ratio, std::mt19937 &rng) {
    int64_t max_deficit = *std::max_element(deficits.begin(), deficits.end());
    int64_t threshold = static_cast<int64_t>(static_cast<double>(max_deficit)
                                             *threshold_ratio);
    Counters priority;
    int network_size = static_cast<int>(queues_deadline_heap.size());
    for (int i = 0; i < network_size; ++i) {
        if (queues_deadline_heap[i].empty()) {
            priority.push_back(0);
                // priority does not matter for unavailable links
        } else {
            if (deficits[i] >= threshold) {
                    // do EDF for those with deficit >= threshold
                int remaining_delay_bound =
                    queues_deadline_heap[i][0].deadline()-current_time+1;
                priority.push_back(max_delay_bound+1-remaining_delay_bound);
                    // the priority is guaranteed positive, so those with
                    // deficits >= threshold have relative higher priority than
                    // those with deficits < threshold
            } else {  // do LDF for those with deficits < threshold
                priority.push_back(deficits[i]-max_deficit-1);
                    // the priority is guaranteed negative
            }
        }
    }
    return greedy(available_queues(queues_deadline_heap), priority,
                  maximal_schedule_matrix, rng);
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

BooleanVector ldf_vision(const Queues &queues_deadline_heap,
                         const Counters &deficits,
                         const BooleanMatrix &maximal_schedule_matrix,
                         std::mt19937 &rng) {
    // Initialize available_links to be the set of links with available packets.
    IntegerVector available_links = available_queue_set(queues_deadline_heap);
    // Make a map from available_links to the restricted feasible schedules. The
    // schedules need not be maximal, but must be unique.
    BooleanMatrix candidate = restrict_schedule(maximal_schedule_matrix,
                                                available_links);
    // Test all pairs and singletons in available_links and choose best pair or
    // singleton winner.
    IntegerVector survivor = get_indices(candidate);
    IntegerVector unconsidered_links = available_links;
    while (!survivor.empty()) {
        int64_t best_deficit = -1;  // Assuming all deficits are nonnegative.
        IntegerVector best_trial;
        IntegerVector winner;
        // Traverse trial in unconsidered_links.
        IntegerMatrix all_trials = traverse(unconsidered_links);
        std::shuffle(all_trials.begin(), all_trials.end(), rng);
        for (int i = 0; i < all_trials.size(); ++i) {
            IntegerVector trial = all_trials[i];
            // Find the trial with largest deficit in some candidate[survivor],
            // and denote it by best_trial.
            int64_t total_deficit = weighted_sum(deficits, trial);
            if (total_deficit > best_deficit) {
                IntegerVector leader = match(trial, candidate, survivor);
                if (!leader.empty()) {
                    best_trial = trial;
                    winner = leader;
                    best_deficit = total_deficit;
                }
            }
        }
        // Update survivor.
        survivor = winner;
        // If survivor is unique, break. Otherwise repeat test.
        if (survivor.size() == 1) {
            break;
        }
        // Update unconsidered_links.
        remove_elements(unconsidered_links, best_trial);
    }
    // Return the unique survivor maximal schedule.
    return candidate[survivor[0]];
}

IntegerVector available_queue_set(const Queues &q) {
    IntegerVector avail;
    for (int i = 0; i < q.size(); ++i) {
        if (!q[i].empty()) {
            avail.push_back(i);
        }
    }
    return avail;
}

// TODO(Veggente): make it persistent so that the function is only run for each
// available_links once.
BooleanMatrix restrict_schedule(const BooleanMatrix &maximal_schedule_matrix,
                                const IntegerVector &available_links) {
    std::set<BooleanVector> schedule_set;
    int network_size = static_cast<int>(maximal_schedule_matrix[0].size());
    for (int i = 0; i < maximal_schedule_matrix.size(); ++i) {
        BooleanVector temp_schedule(network_size, false);
        for (int j = 0; j < available_links.size(); ++j) {
            temp_schedule[available_links[j]] =
            maximal_schedule_matrix[i][available_links[j]];
        }
        schedule_set.insert(temp_schedule);
    }
    BooleanMatrix schedule(schedule_set.begin(), schedule_set.end());
    return schedule;
}

IntegerVector get_indices(const BooleanMatrix &candidate) {
    IntegerVector indices;
    for (int i = 0; i < candidate.size(); ++i) {
        indices.push_back(i);
    }
    return indices;
}

IntegerMatrix traverse(const IntegerVector &available_links) {
    IntegerMatrix all_singletons_and_doubletons;
    for (int i = 0; i < available_links.size(); ++i) {
        IntegerVector temp_vec{available_links[i]};  // NOLINT
        all_singletons_and_doubletons.push_back(temp_vec);
        temp_vec.push_back(-1);  // Placeholder.
        for (int j = i+1; j < available_links.size(); ++j) {
            temp_vec[1] = available_links[j];
            all_singletons_and_doubletons.push_back(temp_vec);
        }
    }
    return all_singletons_and_doubletons;
}

int64_t weighted_sum(const Counters &weight, const IntegerVector &link_set) {
    int64_t total_weight = 0;
    for (int i = 0; i < link_set.size(); ++i) {
        total_weight += weight[link_set[i]];
    }
    return total_weight;
}

IntegerVector match(const IntegerVector &trial, const BooleanMatrix &candidate,
                    const IntegerVector &survivor) {
    IntegerVector match_result;
    for (int i = 0; i < survivor.size(); ++i) {
        bool indicator = true;
        for (int j = 0; j < trial.size(); ++j) {
            if (!candidate[survivor[i]][trial[j]]) {
                indicator = false;
            }
        }
        if (indicator) {
            match_result.push_back(survivor[i]);
        }
    }
    return match_result;
}

void remove_elements(IntegerVector &original_set,  // NOLINT
                     const IntegerVector &elements) {
    for (int i = 0; i < elements.size(); ++i) {
        original_set.erase(std::remove(original_set.begin(), original_set.end(),
                                       elements[i]), original_set.end());
    }
}

