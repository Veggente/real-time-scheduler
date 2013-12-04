//
//  simulator.cpp
//  real-time-scheduler
//
//  Copyright (c) 2013 Xiaohan Kang. All rights reserved.
//

#include "./simulator.h"
#include <random>
#include <iostream>  // NOLINT
#include <fstream>  // NOLINT
#include <string>
#include <vector>
#include "./network_generator.h"
#include "./traffic_generator.h"
#include "./real_time_queueing.h"
#include "/usr/local/include/prettyprint.hpp"

std::string policy_to_string(int p);

int simulator(const std::string &input_file) {
    // Open input file
    std::ifstream in(input_file);
    if (!in) {
        std::cerr << "Error: Cannot open file " << input_file << "!"
            << std::endl;
        exit(1);
    }

    // Variable declarations
    std::string network_type_string;
    NetworkType network_type;
    int network_size;
    int interference_radius;
    BooleanMatrix maximal_schedule_matrix;
    std::string arrival_dist_string;
    ArrivalDistribution arrival_dist;
    int min_packet = 0;  // allow zero arrival
    int max_packet;
    double binom_param;
    std::string arrival_descriptor;
    int min_delay_bound;
    int max_delay_bound;
    int bandwidth_count;
    IntegerVector bandwidths;
    Ratios qos;
    int num_iterations;

    // Read variables from input file
    in >> network_type_string >> network_size;
    if (network_type_string == "collocated") {
        network_type = COLLOCATED;
        maximal_schedule_matrix = gen_max_matrix_collocated(network_size);
    } else if (network_type_string == "line") {
        in >> interference_radius;
        network_type = LINE;
        maximal_schedule_matrix = gen_max_matrix_line(network_size,
                                                      interference_radius);
    } else if (network_type_string == "cycle") {
        in >> interference_radius;
        network_type = CYCLE;
        maximal_schedule_matrix = gen_max_matrix_cycle(network_size,
                                                       interference_radius);
    } else {
        std::cerr << "Error: Network type " << network_type_string
            << " not recognized!" << std::endl;
        exit(1);
    }
    in >> arrival_dist_string >> max_packet;
    if (arrival_dist_string == "uniform") {
        arrival_dist = UNIFORM_PACKET;
        arrival_descriptor = "uniform("+std::to_string(min_packet)+", "
            +std::to_string(max_packet)+")";
    } else if (arrival_dist_string == "binomial") {
        arrival_dist = BINOMIAL_PACKET;
        in >> binom_param;
        arrival_descriptor = "binomial("+std::to_string(max_packet)+", "
            +std::to_string(binom_param)+")";
    } else {
        std::cerr << "Error: Arrival distribution " << arrival_dist_string
            << " not recognized!" << std::endl;
        exit(1);
    }
    in >> min_delay_bound >> max_delay_bound >> bandwidth_count;
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

    // Initialize systems
    std::mt19937 rng;
    std::vector<std::vector<QueueingSystem>> queueing_systems;
    std::vector<std::vector<std::string>> queueing_system_names;
        // per-policy, per-bandwidth systems
    for (int i = 0; i < POLICY_COUNT; ++i) {
        std::vector<QueueingSystem> temp_system_vector;
        std::vector<std::string> temp_name_vector;
        for (int j = 0; j < bandwidth_count; ++j) {
            QueueingSystem temp_system(maximal_schedule_matrix,
                                       static_cast<Policy>(i) , qos,
                                       bandwidths[j], max_delay_bound);
            temp_system_vector.push_back(temp_system);
            std::string temp_name = network_type_string.substr(0, 2)
                // TODO(Veggente): magic #
                +std::to_string(network_size);
            if ( (network_type == LINE) || (network_type == CYCLE) ) {
                temp_name.append("-i"+std::to_string(interference_radius));
            }
            temp_name.append("-du"+std::to_string(max_delay_bound)+"-dl"
                             +std::to_string(min_delay_bound)+"-it"
                             +std::to_string(num_iterations)+policy_to_string(i)
                             +"-b"+std::to_string(bandwidths[j])+".txt");
            temp_name_vector.push_back(temp_name);

            // Output network configuration to file
            std::ofstream out(temp_name, std::ofstream::app);
            if (!out) {
                std::cerr << "Error: Could not open file " << temp_name << "!"
                << std::endl;
                exit(1);
            }
            out << "============Network Configuration============" << std::endl;
            out << "Network type: " << network_type_string << std::endl;
            out << "Network size: " << network_size << std::endl;
            if ( (network_type == LINE) || (network_type == CYCLE) ) {
                out << "Interference radius: " << interference_radius
                    << std::endl;
            }
            out << "Policy: " << policy_to_string(i) << std::endl;
            out << "Max delay bound: " << max_delay_bound << std::endl;
            out << "Min delay bound: " << min_delay_bound << std::endl;
            out << "Bandwidth: " << bandwidths[j] << std::endl;
            out << "Arrival distribution: " << arrival_descriptor << std::endl;
            out << "Delay bound distribution: uniform(" << min_delay_bound
                << ", " << max_delay_bound << ")" << std::endl;
            out << "QoS: " << qos << std::endl;
            out << "Number of iterations: " << num_iterations << std::endl;
            out << std::endl;
            out << "==================Deficits===================" << std::endl;
            out.close();
        }
        queueing_systems.push_back(temp_system_vector);
        queueing_system_names.push_back(temp_name_vector);
    }

    // Network evolution
    for (int time_slot = 0; time_slot < num_iterations; ++time_slot) {
        Traffic traffic;
        if (arrival_dist == BINOMIAL_PACKET) {
            traffic = generate_binomial_traffic(network_size, time_slot,
                                                max_packet, binom_param,
                                                min_delay_bound,
                                                max_delay_bound, rng);
        } else if (arrival_dist == UNIFORM_PACKET) {
            traffic = generate_uniform_traffic(network_size, time_slot,
                                               min_packet, max_packet,
                                               min_delay_bound, max_delay_bound,
                                               rng);
        } else {
            // should not happen
        }
        for (int i = 0; i < POLICY_COUNT; ++i) {
            for (int j = 0; j < bandwidth_count; ++j) {
                queueing_systems[i][j].arrive(traffic, rng);
                queueing_systems[i][j].depart(rng);
                queueing_systems[i][j].output_deficits(
                    queueing_system_names[i][j]);
                queueing_systems[i][j].clock_tick();
            }
        }
    }
    return 0;
}

std::string policy_to_string(int p) {
    switch (p) {
        case LDF:
            return "ldf";
        case EDF:
            return "edf";
        case SDBF:
            return "sdbf";

        default:
            return "";
            break;
    }
}
