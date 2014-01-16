//
//  common.h
//  real-time-scheduler
//
//  Copyright (c) 2013-2014 Xiaohan Kang. All rights reserved.
//

#ifndef REAL_TIME_SCHEDULER_COMMON_H_
#define REAL_TIME_SCHEDULER_COMMON_H_

#include <vector>
#include <map>
#include <utility>
#include <string>
#include <iostream>  // NOLINT

enum Policy {
    LDF,  // largest-deficit-first
    EDF,  // earliest-deadline-first
    SDBF,  // shortest-delay-bound-first
    EDF_NAIVE,  // deficit-unaware EDF
    SDBF_NAIVE,  // deficit-unaware SDBF
    MAXIMAL,  // random maximal
    POLICY_COUNT
};

enum TieBreaker {
    RANDOM,  // uniformly random tie-breaker
    DEADLINE,  // earliest-deadline-first tie-breaker
    DELAY_BOUND,  // shortest-delay-bound-first tie-breaker
    TIE_BREAKER_COUNT
};

enum NetworkType {
    COLLOCATED,
    LINE,
    CYCLE,
    UNIT_DISK,
    NETWORK_TYPE_COUNT
};

enum ArrivalDistribution {
    UNIFORM_PACKET,
    BINOMIAL_PACKET,
    BERNOULLI_PACKET,
    ARRIVAL_DIST_COUNT
};

class Packet;
class QueueingSystem;
typedef std::vector<Packet> PacketSet;
typedef std::vector<PacketSet> Traffic;
typedef std::vector<PacketSet> Queues;
typedef std::vector<double> Ratios;
typedef std::vector<int> IntegerVector;
typedef std::vector<long> Counters;
typedef std::vector<PacketSet> Queues;
typedef std::vector<bool> BooleanVector;
typedef std::vector<std::vector<bool>> BooleanMatrix;
typedef std::map<int, BooleanMatrix> BooleanMatrixMap;
typedef std::pair<int, int> IndexPair;  // (priority, index) pair
typedef std::vector<IndexPair> IndexMapping;
    // (priority, index) mapping
typedef std::vector<std::vector<std::vector<QueueingSystem>>> QueueingSystem3D;

const int HUNDRED = 100;
const int NETWORK_TYPE_ID_LEN = 2;

inline void cannot_open_file(const std::string &filename) {
    std::cerr << "Error: Cannot open file " << filename << "!"
    << std::endl;
    exit(1);
}

#endif  // REAL_TIME_SCHEDULER_COMMON_H_
