//
//  policy.cpp
//  real-time-scheduler
//
//  Copyright (c) 2013 Xiaohan Kang. All rights reserved.
//

#include "./policy.h"

BooleanVector greedy(const BooleanVector &availability,
                     const IntegerVector &priority,
                     const BooleanMatrix &maximal_schedule_matrix,
                     std::mt19937 &rng) {
    BooleanVector schedule;  // TODO(Veggente): implement greedy using lqf()
    return schedule;
}

BooleanVector ldf(const Queues &queues_deadline_heap,
                  const Counters &deficits,
                  const BooleanMatrix &maximal_schedule_matrix,
                  std::mt19937 &rng) {
    BooleanVector schedule;  // TODO(Veggente): implement ldf using greedy()
    return schedule;
}

BooleanVector edf(const Queues &queues_deadline_heap,
                  const Counters &deficits,
                  const BooleanMatrix &maximal_schedule_matrix,
                  int max_deadline, std::mt19937 &rng) {
    BooleanVector schedule;  // TODO(Veggente): implement edf using greedy()
    return schedule;
}

BooleanVector sdbf(const Queues &queues_delay_bound_heap,
                   const Counters &deficits,
                   const BooleanMatrix &maximal_schedule_matrix,
                   int max_delay_bound, std::mt19937 &rng) {
    BooleanVector schedule;  // TODO(Veggente): implement sdbf using greedy()
    return schedule;
}
