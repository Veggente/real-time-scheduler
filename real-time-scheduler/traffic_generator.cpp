//
//  traffic_generator.cpp
//  real-time-scheduler
//
//  Copyright (c) 2013 Xiaohan Kang. All rights reserved.
//

#include "./traffic_generator.h"
#include "./real_time_queueing.h"

Traffic generate_uniform_traffic(int network_size, int current_time,
                                 int min_packet, int max_packet,
                                 int min_delay_bound, int max_delay_bound,
                                 std::mt19937 &rng) {
    PacketSet empty_packet_set;
    Traffic traffic(network_size, empty_packet_set);
    std::uniform_int_distribution<> unif_packet(min_packet, max_packet);
    std::uniform_int_distribution<> unif_delay_bound(min_delay_bound,
                                                     max_delay_bound);
    for (int i = 0; i < network_size; ++i) {
        for (int j = 0; j < unif_packet(rng); ++j) {
            Packet p(current_time, current_time+unif_delay_bound(rng)-1);
            traffic[i].push_back(p);
        }
    }
    return traffic;
}

Traffic generate_binomial_traffic(int network_size, int current_time,
                                  int max_packet, double binom_param,
                                  int min_delay_bound, int max_delay_bound,
                                  std::mt19937 &rng) {
    PacketSet empty_packet_set;
    Traffic traffic(network_size, empty_packet_set);
    std::binomial_distribution<> binom_packet(max_packet, binom_param);
    std::uniform_int_distribution<> unif_delay_bound(min_delay_bound,
                                                     max_delay_bound);
    for (int i = 0; i < network_size; ++i) {
        for (int j = 0; j < binom_packet(rng); ++j) {
            Packet p(current_time, current_time+unif_delay_bound(rng)-1);
            traffic[i].push_back(p);
        }
    }
    return traffic;
}
