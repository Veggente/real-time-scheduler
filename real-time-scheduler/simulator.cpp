//
//  simulator.cpp
//  real-time-scheduler
//
//  Copyright (c) 2013 Xiaohan Kang. All rights reserved.
//

#include "./simulator.h"
#include <iostream>
#include <random>
#include <fstream>
#include "./network_generator.h"
#include "./traffic_generator.h"
#include "./real_time_queueing.h"
#include "/usr/local/include/prettyprint.hpp"

std::string arrival_dist_string(ArrivalDistribution arr_dist, int max_packet,
                                double dist_param);

int simulator(int network_size, int num_iterations) {  // obsolete
    int interference_radius = 1;
    BooleanMatrix maximal_schedule_matrix =
        gen_max_matrix_line(network_size, interference_radius);
    // int min_packet = 0;
    int max_packet = 2;
    int min_delay_bound = 2;
    int max_delay_bound = 4;
    double binom_param = 0.5;
    Ratios qos(network_size, 0.53);
    int bandwidth = 1;
    std::string ldf_deficit_filename = "ldf.txt";
    std::mt19937 rng;
    QueueingSystem system_ldf(maximal_schedule_matrix, LDF, qos, bandwidth,
                              max_delay_bound);
    for (int time_slot = 0; time_slot < num_iterations; ++time_slot) {
//        Traffic traffic = generate_uniform_traffic(network_size, time_slot,
//                                                   min_packet, max_packet,
//                                                   min_delay_bound,
//                                                   max_delay_bound, rng);
        Traffic traffic = generate_binomial_traffic(network_size, time_slot,
                                                    max_packet, binom_param,
                                                    min_delay_bound,
                                                    max_delay_bound, rng);
        system_ldf.arrive(traffic, rng);
        system_ldf.depart(rng);
        system_ldf.output_deficits(ldf_deficit_filename);
        system_ldf.clock_tick();
    }
    return 0;
}

int simulator_collocated(int network_size, ArrivalDistribution arr_dist,
                         int max_packet, double dist_param, int min_delay_bound,
                         int max_delay_bound, const IntegerVector &bandwidths,
                         const Ratios &qos, int num_iterations) {
    BooleanMatrix maximal_schedule_matrix =
        gen_max_matrix_collocated(network_size);
    int min_packet = 0;
    std::mt19937 rng;
    std::vector<QueueingSystem> system_ldf;
    std::vector<QueueingSystem> system_edf;
    std::vector<QueueingSystem> system_sdbf;
    std::vector<std::string> filename_ldf;
    std::vector<std::string> filename_edf;
    std::vector<std::string> filename_sdbf;
    for (int i = 0; i < bandwidths.size() ; ++i) {
        QueueingSystem subsystem_ldf(maximal_schedule_matrix, LDF, qos,
                                     bandwidths[i], max_delay_bound);
        QueueingSystem subsystem_edf(maximal_schedule_matrix, EDF, qos,
                                     bandwidths[i], max_delay_bound);
        QueueingSystem subsystem_sdbf(maximal_schedule_matrix, SDBF, qos,
                                      bandwidths[i], max_delay_bound);
        system_ldf.push_back(subsystem_ldf);
        system_edf.push_back(subsystem_edf);
        system_sdbf.push_back(subsystem_sdbf);
        filename_ldf.push_back("n"+std::to_string(network_size)+"-co-ldf-d"
                               +std::to_string(max_delay_bound)+"-b"
                               +std::to_string(bandwidths[i])+".txt");
        filename_edf.push_back("n"+std::to_string(network_size)+"-co-edf-d"
                               +std::to_string(max_delay_bound)+"-b"
                               +std::to_string(bandwidths[i])+".txt");
        filename_sdbf.push_back("n"+std::to_string(network_size)+"-co-sdbf-d"
                                +std::to_string(max_delay_bound)+"-b"
                                +std::to_string(bandwidths[i])+".txt");
        // TODO(Veggente): encapsulation of the following output
        std::ofstream out(filename_ldf[i], std::ofstream::app);
        if (!out) {
            std::cerr << "Error: Could not open file " << filename_ldf[i] << "!"
            << std::endl;
            exit(1);
        }
        out << "==============Network Configuration==============" << std::endl;
        out << "Network type: collocated" << std::endl;
        out << "Network size: " << network_size << std::endl;
        out << "Policy: LDF" << std::endl;
        out << "Maximum delay bound: " << max_delay_bound << std::endl;
        out << "Bandwidth: " << bandwidths[i] << std::endl;
        out << "Arrival distribution: "
            +arrival_dist_string(arr_dist, max_packet, dist_param)
            << std::endl;
        out << "Delay bound distribution: uniform(" << min_delay_bound << ", "
            << max_delay_bound << ")" << std::endl;
        out << "QoS: " << qos << std::endl;
        out << "====================Deficits=====================" << std::endl;
        out.close();
        out.open(filename_edf[i]);
        if (!out) {
            std::cerr << "Error: Could not open file " << filename_edf[i] << "!"
            << std::endl;
            exit(1);
        }
        out << "==============Network Configuration==============" << std::endl;
        out << "Network type: collocated" << std::endl;
        out << "Network size: " << network_size << std::endl;
        out << "Policy: EDF" << std::endl;
        out << "Maximum delay bound: " << max_delay_bound << std::endl;
        out << "Bandwidth: " << bandwidths[i] << std::endl;
        out << "Arrival distribution: "
            +arrival_dist_string(arr_dist, max_packet, dist_param)
            << std::endl;
        out << "Delay bound distribution: uniform(" << min_delay_bound << ", "
            << max_delay_bound << ")" << std::endl;
        out << "QoS: " << qos << std::endl;
        out << "====================Deficits=====================" << std::endl;
        out.close();
        out.open(filename_sdbf[i]);
        if (!out) {
            std::cerr << "Error: Could not open file " << filename_sdbf[i]
                << "!" << std::endl;
            exit(1);
        }
        out << "==============Network Configuration==============" << std::endl;
        out << "Network type: collocated" << std::endl;
        out << "Network size: " << network_size << std::endl;
        out << "Policy: SDBF" << std::endl;
        out << "Maximum delay bound: " << max_delay_bound << std::endl;
        out << "Bandwidth: " << bandwidths[i] << std::endl;
        out << "Arrival distribution: "
            +arrival_dist_string(arr_dist, max_packet, dist_param)
            << std::endl;
        out << "Delay bound distribution: uniform(" << min_delay_bound << ", "
            << max_delay_bound << ")" << std::endl;
        out << "QoS: " << qos << std::endl;
        out << "====================Deficits=====================" << std::endl;
        out.close();
    }
    for (int time_slot = 0; time_slot < num_iterations; ++time_slot) {
        Traffic traffic;
        if (arr_dist == BINOMIAL_PACKET) {
            traffic = generate_binomial_traffic(network_size, time_slot,
                                                max_packet, dist_param,
                                                min_delay_bound,
                                                max_delay_bound, rng);
        } else if (arr_dist == UNIFORM_PACKET) {
            traffic = generate_uniform_traffic(network_size, time_slot,
                                               min_packet, max_packet,
                                               min_delay_bound, max_delay_bound,
                                               rng);
        } else {
            // should not happen
        }
        for (int i = 0; i < bandwidths.size(); ++i) {
            system_ldf[i].arrive(traffic, rng);
            system_edf[i].arrive(traffic, rng);
            system_sdbf[i].arrive(traffic, rng);
            system_ldf[i].depart(rng);
            system_edf[i].depart(rng);
            system_sdbf[i].depart(rng);
            system_ldf[i].output_deficits(filename_ldf[i]);
            system_edf[i].output_deficits(filename_edf[i]);
            system_sdbf[i].output_deficits(filename_sdbf[i]);
            system_ldf[i].clock_tick();
            system_edf[i].clock_tick();
            system_sdbf[i].clock_tick();
        }
    }
    return 0;
}

ArrivalDistribution string_to_arr_dist(std::string s) {
    if (s == "uniform") {
        return UNIFORM_PACKET;
    } else if (s == "binomial") {
        return BINOMIAL_PACKET;
    } else {
        std::cerr << "Arrival distribution not recognized! Please use uniform or binomial!" << std::endl;
        exit(1);
    }
}

std::string arrival_dist_string(ArrivalDistribution arr_dist, int max_packet,
                                double dist_param) {
    if (arr_dist == UNIFORM_PACKET) {
        return "uniform(0, "+std::to_string(max_packet)+")";
    } else {  // TODO(Veggente): extensibility
        return "binomial("+std::to_string(max_packet)+", "
            +std::to_string(dist_param)+")";
    }
}
