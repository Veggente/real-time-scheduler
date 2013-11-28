//
//  real_time_queueing.cpp
//  real-time-scheduler
//
//  Copyright (c) 2013 Xiaohan Kang. All rights reserved.
//

#include "./real_time_queueing.h"

QueueingSystem::QueueingSystem(int i, Policy s) {
    network_size_ = i;
    per_link_deficit_.insert(per_link_deficit_.begin(), network_size_, 0);
    PacketSet empty_packet_set;
    per_link_queue_.insert(per_link_queue_.begin(), network_size_,
                           empty_packet_set);
    scheduler_ = s;
}
