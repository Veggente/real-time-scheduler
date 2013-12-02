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

class Packet;
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

#endif  // REAL_TIME_SCHEDULER_COMMON_H_
