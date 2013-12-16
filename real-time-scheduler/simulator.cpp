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
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include "./network_generator.h"
#include "./traffic_generator.h"
#include "./real_time_queueing.h"
#include "/usr/local/include/prettyprint.hpp"

std::string policy_to_string(int p);
void progress_bar(int time_slot, int num_iterations);
void cannot_open_file(const std::string &filename);

int simulator(const std::string &input_file) {
    // Open input file
    std::ifstream in(input_file);
    if (!in) {
        cannot_open_file(input_file);
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
    int ratio_count;
    Ratios ratios;
    int num_iterations;

    // Read variables from input file
    // TODO(Veggente): format compatibility check
    in.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    in >> network_type_string;
    in.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    in >> network_size;
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
    in.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    in >> arrival_dist_string;
    in.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    in >> max_packet;
    if (arrival_dist_string == "uniform") {
        arrival_dist = UNIFORM_PACKET;
        arrival_descriptor = "uniform("+std::to_string(min_packet)+", "
            +std::to_string(max_packet)+")";
    } else if (arrival_dist_string == "binomial") {
        arrival_dist = BINOMIAL_PACKET;
        in >> binom_param;
        std::ostringstream ostr;
        ostr << binom_param;
        arrival_descriptor = "binomial("+std::to_string(max_packet)+", "
            +ostr.str()+")";
    } else {
        std::cerr << "Error: Arrival distribution " << arrival_dist_string
            << " not recognized!" << std::endl;
        exit(1);
    }
    in.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    in >> min_delay_bound;
    in.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    in >> max_delay_bound;
    in.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    in >> bandwidth_count;
    in.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    for (int i = 0; i < bandwidth_count; ++i) {
        int temp;
        in >> temp;
        bandwidths.push_back(temp);
    }
    in.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    for (int i = 0; i < network_size; ++i) {
        double temp;
        in >> temp;
        qos.push_back(temp);
    }
    double max_qos = *std::max_element(qos.begin(), qos.end());
    for (int i = 0; i < network_size; ++i) {
        qos[i] = qos[i] / max_qos;
            // normalization since qos cannot be greater than 1
    }
    in.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    in >> ratio_count;
    in.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    for (int i = 0; i < ratio_count; ++i) {
        double temp;
        in >> temp;
        ratios.push_back(temp);
    }
    in.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    in >> num_iterations;
    in.close();

    // Initialize systems and names
    std::mt19937 rng;
    std::vector<std::vector<std::vector<QueueingSystem>>> queueing_systems;
    std::vector<std::vector<std::vector<std::string>>> queueing_system_names;
        // per-policy, per-bandwidth, per-qos-ratio systems
    for (int policy_it = 0; policy_it < POLICY_COUNT; ++policy_it) {
        std::vector<std::vector<QueueingSystem>> temp_system_matrix;
        std::vector<std::vector<std::string>> temp_name_matrix;
        for (int bandwidth_it = 0; bandwidth_it < bandwidth_count;
             ++bandwidth_it) {
            std::vector<QueueingSystem> temp_system_vector;
            std::vector<std::string> temp_name_vector;
            for (int ratio_it = 0; ratio_it < ratio_count; ++ratio_it) {
                Ratios scaled_qos = qos;
                for (int k = 0; k < network_size; ++k) {
                    scaled_qos[k] = scaled_qos[k]*ratios[ratio_it];
                }
                QueueingSystem temp_system(maximal_schedule_matrix,
                                           static_cast<Policy>(policy_it),
                                           scaled_qos, bandwidths[bandwidth_it],
                                           max_delay_bound);
                temp_system_vector.push_back(temp_system);
                std::string temp_name = network_type_string.substr(0, 2)
                    // TODO(Veggente): magic # 2
                    +std::to_string(network_size);
                if ( (network_type == LINE) || (network_type == CYCLE) ) {
                    temp_name.append("-i"+std::to_string(interference_radius));
                }
                std::ostringstream ostr;
                ostr << ratios[ratio_it];
                temp_name.append("-du"+std::to_string(max_delay_bound)+"-dl"
                                 +std::to_string(min_delay_bound)+"-it"
                                 +std::to_string(num_iterations)+"-"
                                 +policy_to_string(policy_it)+"-b"
                                 +std::to_string(bandwidths[bandwidth_it])+"-r"
                                 +ostr.str()+".txt");
                temp_name_vector.push_back(temp_name);

                // Output network configuration to file
                std::ofstream out(temp_name, std::ofstream::trunc);
                if (!out) {
                    std::cerr << "Error: Could not open file " << temp_name
                        << "!" << std::endl;
                    exit(1);
                }
                out << "============Network Configuration============"
                    << std::endl;
                out << "Network type: " << network_type_string << std::endl;
                out << "Network size: " << network_size << std::endl;
                if ( (network_type == LINE) || (network_type == CYCLE) ) {
                    out << "Interference radius: " << interference_radius
                        << std::endl;
                }
                out << "Policy: " << policy_to_string(policy_it) << std::endl;
                out << "Max delay bound: " << max_delay_bound << std::endl;
                out << "Min delay bound: " << min_delay_bound << std::endl;
                out << "Bandwidth: " << bandwidths[bandwidth_it] << std::endl;
                out << "Arrival distribution: " << arrival_descriptor
                    << std::endl;
                out << "Delay bound distribution: uniform(" << min_delay_bound
                    << ", " << max_delay_bound << ")" << std::endl;
                out << "Base QoS: " << qos << std::endl;
                out << "QoS scalar: " << ratios[ratio_it] << std::endl;
                out << "Number of iterations: " << num_iterations << std::endl;
                out << std::endl;
                out << "==================Deficits==================="
                    << std::endl;
                out.close();
            }
            temp_system_matrix.push_back(temp_system_vector);
            temp_name_matrix.push_back(temp_name_vector);
        }
        queueing_systems.push_back(temp_system_matrix);
        queueing_system_names.push_back(temp_name_matrix);
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
            // TODO(Veggente): should not happen
        }
        for (int i = 0; i < POLICY_COUNT; ++i) {
            for (int j = 0; j < bandwidth_count; ++j) {
                for (int ratio_it = 0; ratio_it < ratio_count; ++ratio_it) {
                    queueing_systems[i][j][ratio_it].arrive(traffic, rng);
                    queueing_systems[i][j][ratio_it].depart(rng);
                    queueing_systems[i][j][ratio_it].output_deficits(
                        queueing_system_names[i][j][ratio_it]);
                    queueing_systems[i][j][ratio_it].clock_tick();
                }
            }
        }
        progress_bar(time_slot, num_iterations);
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

void progress_bar(int time_slot, int num_iterations) {
    if (time_slot == num_iterations-1) {
        std::cout << "\rDone!           " << std::endl;;
    } else if (time_slot*HUNDRED/num_iterations !=
               (time_slot-1)*HUNDRED/num_iterations) {
        std::cout << "\r" << time_slot*HUNDRED/num_iterations << "% completed"
            << std::flush;
    }
}

void cannot_open_file(const std::string &filename) {
    std::cerr << "Error: Cannot open file " << filename << "!"
        << std::endl;
    exit(1);
}
