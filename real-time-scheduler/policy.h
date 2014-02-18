//
//  policy.h
//  real-time-scheduler
//
//  Copyright (c) 2013-2014 Xiaohan Kang. All rights reserved.
//

#ifndef REAL_TIME_SCHEDULER_POLICY_H_
#define REAL_TIME_SCHEDULER_POLICY_H_

#include <random>
#include "./common.h"

BooleanVector greedy(const BooleanVector &availability,
                     const Counters &priority,
                     const BooleanMatrix &maximal_schedule_matrix,
                         // must be maximal
                     std::mt19937 &rng);

BooleanVector ldf(const Queues &queues_deadline_heap,  // min deadline heap
                  const Counters &deficits,
                  const BooleanMatrix &maximal_schedule_matrix,
                  std::mt19937 &rng);

BooleanVector edf(const Queues &queues_deadline_heap,  // min deadline heap
                  const Counters &deficits,
                  const BooleanMatrix &maximal_schedule_matrix,
                  int current_time, int max_delay_bound, std::mt19937 &rng);

BooleanVector sdbf(const Queues &queues_delay_bound_heap,  // min delay bound
                                                           // heap
                   const Counters &deficits,
                   const BooleanMatrix &maximal_schedule_matrix,
                   int max_delay_bound, std::mt19937 &rng);

BooleanVector edf_naive(const Queues &queues_deadline_heap,
                        const BooleanMatrix &maximal_schedule_matrix,
                        int current_time, std::mt19937 &rng);

BooleanVector sdbf_naive(const Queues &queues_deadline_heap,
                         const BooleanMatrix &maximal_schedule_matrix,
                         std::mt19937 &rng);

BooleanVector maximal(const Queues &queues_deadline_heap,
                      const BooleanMatrix &maximal_schedule_matrix,
                      std::mt19937 &rng);

BooleanVector ldf_threshold(const Queues &queues_deadline_heap,
                            const Counters &deficits,
                            const BooleanMatrix &maximal_schedule_matrix,
                            int current_time, int max_delay_bound,
                            double threshold_ratio, std::mt19937 &rng);

#endif  // REAL_TIME_SCHEDULER_POLICY_H_
