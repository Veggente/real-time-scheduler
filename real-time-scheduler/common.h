//
//  common.h
//  real-time-scheduler
//
//  Copyright (c) 2013 Xiaohan Kang. All rights reserved.
//

#ifndef REAL_TIME_SCHEDULER_COMMON_H_
#define REAL_TIME_SCHEDULER_COMMON_H_

#include <vector>
#include <map>
#include <utility>
#include <string>

enum Policy {
    LDF,  // largest-deficit-first
    EDF,  // earliest-deadline-first
    SDBF,  // shortest-delay-bound-first
    POLICY_COUNT
};

enum TieBreaker {
    RANDOM,  // uniformly random tie-breaker
    DEADLINE,  // earliest-deadline-first tie-breaker
    DELAY_BOUND  // shortest-delay-bound-first tie-breaker
};

enum NetworkType {
    COLLOCATED,
    LINE,
    CYCLE
};

enum ArrivalDistribution {
    UNIFORM_PACKET,
    BINOMIAL_PACKET
};

class Packet;
class QueueingSystem;
typedef std::vector<Packet> PacketSet;
typedef std::vector<PacketSet> Traffic;
typedef std::vector<PacketSet> Queues;
typedef std::vector<double> Ratios;
typedef std::vector<int> IntegerVector;
typedef std::vector<int> Counters;  // must be nonnegative
typedef std::vector<PacketSet> Queues;
typedef std::vector<bool> BooleanVector;
typedef std::vector<std::vector<bool>> BooleanMatrix;
typedef std::map<int, BooleanMatrix> BooleanMatrixMap;
typedef std::pair<int, int> IndexPair;  // (priority, index) pair
typedef std::vector<IndexPair> IndexMapping;
    // (priority, index) mapping
typedef std::vector<std::vector<std::vector<QueueingSystem>>> QueueingSystem3D;
typedef std::vector<std::vector<std::vector<std::string>>> String3D;

const int HUNDRED = 100;

#endif  // REAL_TIME_SCHEDULER_COMMON_H_
