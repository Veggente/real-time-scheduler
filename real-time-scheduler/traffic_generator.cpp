//
//  traffic_generator.cpp
//  real-time-scheduler
//
//  Copyright (c) 2013-2014 Xiaohan Kang. All rights reserved.
//

#include "./traffic_generator.h"
#include "./real_time_queueing.h"

Traffic generate_uniform_traffic(int network_size, int current_time,
                                 IntegerVector min_packet,
                                 IntegerVector max_packet,
                                 IntegerVector min_delay_bound,
                                 IntegerVector max_delay_bound,
                                 std::mt19937 &rng) {  // NOLINT
    Traffic traffic(network_size, PacketSet(0));
    for (int i = 0; i < network_size; ++i) {
        std::uniform_int_distribution<> unif_packet(min_packet[i],
                                                    max_packet[i]);
        std::uniform_int_distribution<> unif_delay_bound(min_delay_bound[i],
                                                         max_delay_bound[i]);
        for (int j = 0; j < unif_packet(rng); ++j) {
            Packet p(current_time, current_time+unif_delay_bound(rng)-1);
            traffic[i].push_back(p);
        }
    }
    return traffic;
}

Traffic generate_binomial_traffic(int network_size, int current_time,
                                  IntegerVector max_packet, Ratios binom_param,
                                  IntegerVector min_delay_bound,
                                  IntegerVector max_delay_bound,
                                  std::mt19937 &rng) {  // NOLINT
    Traffic traffic(network_size, PacketSet(0));
    for (int i = 0; i < network_size; ++i) {
        std::binomial_distribution<> binom_packet(max_packet[i],
                                                  binom_param[i]);
        std::uniform_int_distribution<> unif_delay_bound(min_delay_bound[i],
                                                         max_delay_bound[i]);
        for (int j = 0; j < binom_packet(rng); ++j) {
            Packet p(current_time, current_time+unif_delay_bound(rng)-1);
            traffic[i].push_back(p);
        }
    }
    return traffic;
}

Traffic generate_bernoulli_traffic(int network_size, int current_time,
                                   IntegerVector min_packet,
                                   IntegerVector max_packet, Ratios bern_param,
                                   IntegerVector min_delay_bound,
                                   IntegerVector max_delay_bound,
                                   std::mt19937 &rng) {
    Traffic traffic(network_size, PacketSet(0));
    for (int i = 0; i < network_size; ++i) {
        std::bernoulli_distribution bern_packet(bern_param[i]);
        std::uniform_int_distribution<> unif_delay_bound(min_delay_bound[i],
                                                         max_delay_bound[i]);
        int num_packet;
        if (bern_packet(rng)) {
            num_packet = max_packet[i];
        } else {
            num_packet = min_packet[i];
        }
        for (int j = 0; j < num_packet; ++j) {
            Packet p(current_time, current_time+unif_delay_bound(rng)-1);
            traffic[i].push_back(p);
        }
    }
    return traffic;
}
