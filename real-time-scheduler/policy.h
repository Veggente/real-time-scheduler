//
//  policy.h
//  real-time-scheduler
//
//  Copyright (c) 2013 Xiaohan Kang. All rights reserved.
//

#ifndef REAL_TIME_SCHEDULER_POLICY_H_
#define REAL_TIME_SCHEDULER_POLICY_H_

#include <random>
#include "./common.h"

enum Policy {   LDF,  // largest-deficit-first
                EDF,  // earliest-deadline-first
                SDBF  // shortest-delay-bound-first
};

enum TieBreaker {   RANDOM,  // uniformly random tie-breaker
                    DEADLINE,  // earliest-deadline-first tie-breaker
                    DELAY_BOUND  // shortest-delay-bound-first tie-breaker
};

BooleanVector greedy(const BooleanVector &availability,
                     const IntegerVector &priority,
                     const BooleanMatrix &maximal_schedule_matrix,
                         // must be maximal
                     std::mt19937 &rng);

BooleanVector ldf(const Queues &queues_deadline_heap,
                  const Counters &deficits,
                  const BooleanMatrix &maximal_schedule_matrix,
                  std::mt19937 &rng);

BooleanVector edf(const Queues &queues_deadline_heap,
                  const Counters &deficits,
                  const BooleanMatrix &maximal_schedule_matrix,
                  int max_deadline, std::mt19937 &rng);

BooleanVector sdbf(const Queues &queues_delay_bound_heap,
                   const Counters &deficits,
                   const BooleanMatrix &maximal_schedule_matrix,
                   int max_delay_bound, std::mt19937 &rng);

#endif  // REAL_TIME_SCHEDULER_POLICY_H_
