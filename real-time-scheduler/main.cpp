//
//  main.cpp
//  real-time-scheduler
//
//  Copyright (c) 2013 Xiaohan Kang. All rights reserved.
//

#include <string>
#include <fstream>
#include "./simulator.h"

int main(int argc, const char * argv[]) {
    // TODO(Veggente): input from file
    std::string input_file(argv[1]);
    std::ifstream in(input_file);
    int network_size;
    std::string arr_dist_string;
    int max_packet;
    double dist_param;
    int min_delay_bound;
    int max_delay_bound;
    int bandwidth_count;
    IntegerVector bandwidths;
    Ratios qos;
    int num_iterations;
    in >> network_size >> arr_dist_string >> max_packet >> dist_param >> min_delay_bound >> max_delay_bound >> bandwidth_count;
    for (int i = 0; i < bandwidth_count; ++i) {
        int temp;
        in >> temp;
        bandwidths.push_back(temp);
    }
    for (int i = 0; i < network_size; ++i) {
        double temp;
        in >> temp;
        qos.push_back(temp);
    }
    in >> num_iterations;
    in.close();
    return simulator_collocated(network_size, string_to_arr_dist(arr_dist_string), max_packet, dist_param, min_delay_bound, max_delay_bound, bandwidths, qos, num_iterations);
}
