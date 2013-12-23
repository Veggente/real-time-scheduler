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
#include "./enum_parser.h"

BooleanVector int_to_bool_vec(int a);  // TODO(Veggente): generalized case

Simulator::Simulator() {
    network_type_ = COLLOCATED;
    network_size_ = 0;
    interference_radius_ = 0;
    maximal_schedule_matrix_ = BooleanMatrix(0);
    arrival_dist_ = UNIFORM_PACKET;
    min_packet_ = IntegerVector(network_size_, 0);
    max_packet_ = IntegerVector(network_size_, 0);
    binom_param_ = Ratios(network_size_, 0);
    min_delay_bound_ = IntegerVector(network_size_, 0);
    max_delay_bound_ = IntegerVector(network_size_, 0);
    base_qos_ = Ratios(network_size_, 0);
    num_iterations_ = 0;
    policy_indicator_ = BooleanVector(POLICY_COUNT, false);
    bandwidth_ = IntegerVector(0);
    qos_ratio_ = Ratios(0);
    queueing_system_ = QueueingSystem3D(0);
    system_clock_ = 0;
}

void Simulator::init(const std::string &config_filename, const std::string &network_filename, std::mt19937 &rng) {
    std::ifstream in(config_filename);
    if (!in) {
        cannot_open_file(config_filename);
    }
    in.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    std::string network_type_string;
    in >> network_type_string;
    EnumParser<NetworkType> parser_net;
    network_type_ = parser_net.string_to_enum(network_type_string);
    in.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    in >> network_size_;  // TODO(Veggente): will be ignored for unit-disk
    switch (network_type_) {
        case COLLOCATED:
            maximal_schedule_matrix_ = gen_max_matrix_collocated(network_size_);
            break;
        case LINE:
            in >> interference_radius_;
            maximal_schedule_matrix_ = gen_max_matrix_line(network_size_,
                interference_radius_);
            break;
        case CYCLE:
            in >> interference_radius_;
            maximal_schedule_matrix_ = gen_max_matrix_cycle(network_size_,
                interference_radius_);
            break;
        case UNIT_DISK:
            maximal_schedule_matrix_ = load_network(network_filename);
            network_size_ =
                static_cast<int>(maximal_schedule_matrix_[0].size());
                // previous assigned network size ignored
            break;
        default:
            std::cerr << "Error: network type not recognized!" << std::endl;
            exit(1);
            break;
    }
    in.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    int timed_seed_indicator;
    in >> timed_seed_indicator;
    if (timed_seed_indicator != 0) {
        rng.seed(static_cast<unsigned int>(std::time(NULL)));
    }
    in.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    std::string arrival_dist_string;
    in >> arrival_dist_string;
    EnumParser<ArrivalDistribution> parser_arr;
    arrival_dist_ = parser_arr.string_to_enum(arrival_dist_string);
    min_packet_ = IntegerVector(network_size_, 0);
    in.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    BooleanVector type_indicator(network_size_/2, false);
    type_indicator.insert(type_indicator.end(),
                          network_size_-network_size_/2, true);
    switch (arrival_dist_) {
        case BINOMIAL_PACKET:
        {
            int max_packet_type_1, max_packet_type_2;
            double binom_param_type_1, binom_param_type_2;
            in >> max_packet_type_1 >> binom_param_type_1
               >> max_packet_type_2 >> binom_param_type_2;
            std::shuffle(type_indicator.begin(), type_indicator.end(), rng);
            for (int i = 0; i < network_size_; ++i) {
                if (type_indicator[i]) {
                    max_packet_.push_back(max_packet_type_1);
                    binom_param_.push_back(binom_param_type_1);
                } else {
                    max_packet_.push_back(max_packet_type_2);
                    binom_param_.push_back(binom_param_type_2);
                }
            }
            break;
        }
        case UNIFORM_PACKET:
        {
            int max_packet_type_1, max_packet_type_2;
            in >> max_packet_type_1 >> max_packet_type_2;
            std::shuffle(type_indicator.begin(), type_indicator.end(), rng);
            for (int i = 0; i < network_size_; ++i) {
                if (type_indicator[i]) {
                    max_packet_.push_back(max_packet_type_1);
                } else {
                    max_packet_.push_back(max_packet_type_2);
                }
            }
            break;
        }
        default:
            std::cerr << "Error: arrival distribution not recognized!"
                      << std::endl;
            exit(1);
            break;
    }
    in.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    int min_delay_bound_type_1, min_delay_bound_type_2;
    in >> min_delay_bound_type_1 >> min_delay_bound_type_2;
    in.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    int max_delay_bound_type_1, max_delay_bound_type_2;
    in >> max_delay_bound_type_1 >> max_delay_bound_type_2;
    int max_delay_bound_overall = std::max(max_delay_bound_type_1,
                                           max_delay_bound_type_2);
    std::shuffle(type_indicator.begin(), type_indicator.end(), rng);
    for (int i = 0; i < network_size_; ++i) {
        if (type_indicator[i]) {
            min_delay_bound_.push_back(min_delay_bound_type_1);
            max_delay_bound_.push_back(max_delay_bound_type_1);
        } else {
            min_delay_bound_.push_back(min_delay_bound_type_2);
            max_delay_bound_.push_back(max_delay_bound_type_2);
        }
    }
    in.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    std::string bandwidth_str;
    std::getline(in, bandwidth_str);
    std::istringstream bandwidth_stream(bandwidth_str);
    std::string word;
    bandwidth_.clear();
    while (bandwidth_stream >> word) {
        bandwidth_.push_back(std::stoi(word));
    }
    in.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    base_qos_.clear();
    if (network_type_ == UNIT_DISK) {
        base_qos_ = Ratios(network_size_, 1);
            // ignore the input file and set all base QoS to 1
    } else {
        for (int i = 0; i < network_size_; ++i) {
            double temp;
            in >> temp;
            base_qos_.push_back(temp);
        }
        double max_qos = *std::max_element(base_qos_.begin(), base_qos_.end());
        for (int i = 0; i < network_size_; ++i) {
            base_qos_[i] = base_qos_[i]/max_qos;
        }
    }
    in.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    std::string ratio_str;
    std::getline(in, ratio_str);
    std::istringstream ratio_stream(ratio_str);
    qos_ratio_.clear();
    while (ratio_stream >> ratio_str) {
        qos_ratio_.push_back(std::stod(ratio_str));
    }
    in.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    in >> num_iterations_;
    in.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    int policy_indicator_int;
    in >> policy_indicator_int;
    policy_indicator_ = int_to_bool_vec(policy_indicator_int);
    in.close();
    queueing_system_.clear();
    for (int policy_it = 0; policy_it < POLICY_COUNT; ++policy_it) {
        std::vector<std::vector<QueueingSystem>> temp_system_matrix;
        for (int bandwidth_it = 0; bandwidth_it < bandwidth_.size();
             ++bandwidth_it) {
            std::vector<QueueingSystem> temp_system_vector;
            for (int ratio_it = 0; ratio_it < qos_ratio_.size(); ++ratio_it) {
                Ratios scaled_qos;
                for (int i = 0; i < network_size_; ++i) {
                    scaled_qos.push_back(base_qos_[i]*qos_ratio_[ratio_it]);
                }
                EnumParser<Policy> parser_pol;
                std::ostringstream ostr;
                ostr << qos_ratio_[ratio_it];
                std::string temp_name =
                    parser_net.enum_to_string(network_type_).substr(0,
                    NETWORK_TYPE_ID_LEN)+std::to_string(network_size_)+"-it"
                    +std::to_string(num_iterations_)+"-"
                    +parser_pol.enum_to_string(static_cast<Policy>(policy_it))
                    +"-b"+std::to_string(bandwidth_[bandwidth_it])+"-r"
                    +ostr.str()+".txt";
                QueueingSystem temp_system(maximal_schedule_matrix_,
                                           static_cast<Policy>(policy_it),
                                           scaled_qos, bandwidth_[bandwidth_it],
                                           max_delay_bound_overall, temp_name,
                                           num_iterations());
                temp_system_vector.push_back(temp_system);
            }
            temp_system_matrix.push_back(temp_system_vector);
        }
        queueing_system_.push_back(temp_system_matrix);
    }
    system_clock_ = 0;
}

void Simulator::save_config() {
    for (int policy_it = 0; policy_it < POLICY_COUNT; ++policy_it) {
        if (!policy_indicator_[policy_it]) {
            continue;
        }
        for (int bandwidth_it = 0; bandwidth_it < bandwidth_.size();
             ++bandwidth_it) {
            for (int ratio_it = 0; ratio_it < qos_ratio_.size(); ++ratio_it) {
                auto &this_system =
                    queueing_system_[policy_it][bandwidth_it][ratio_it];
                std::ofstream out(this_system.output_filename(),
                                  std::ofstream::trunc);
                if (!out) {
                    cannot_open_file(this_system.output_filename());
                }
                out << "============Network Configuration============"
                    << std::endl;
                EnumParser<NetworkType> parser_net;
                out << "Network type: "
                    << parser_net.enum_to_string(network_type_)
                    << std::endl;
                out << "Network size: " << network_size_ << std::endl;
                if ( (network_type_ == LINE) || (network_type_ == CYCLE) ) {
                    out << "Interference radius: " << interference_radius_
                        << std::endl;
                }
                EnumParser<Policy> parser_pol;
                out << "Policy: "
                    << parser_pol.enum_to_string(static_cast<Policy>(policy_it))
                    << std::endl;
                out << "Max delay bound for link 1: " << max_delay_bound_[0]
                    << std::endl;
                out << "Min delay bound for link 1: " << min_delay_bound_[0]
                    << std::endl;
                out << "Bandwidth: " << bandwidth_[bandwidth_it] << std::endl;
                EnumParser<ArrivalDistribution> parser_arr;
                out << "Arrival distribution for link 1: "
                    << parser_arr.enum_to_string(arrival_dist_) << "(";
                if (arrival_dist_ == UNIFORM_PACKET) {
                    out << min_packet_[0] << ", " << max_packet_[0];
                } else if (arrival_dist_ == BINOMIAL_PACKET) {
                    out << max_packet_[0] << ", " << binom_param_[0];
                }
                out << ")" << std::endl;
                out << "Delay bound distribution for link 1: uniform("
                    << min_delay_bound_[0]
                    << ", " << max_delay_bound_[0] << ")" << std::endl;
                out << "Base QoS: " << base_qos_ << std::endl;
                out << "QoS scalar: " << qos_ratio_[ratio_it] << std::endl;
                out << "Number of iterations: " << num_iterations_ << std::endl;
                out << std::endl;
                out << "==================Deficits==================="
                    << std::endl;
                out.close();
            }
        }
    }
}

void Simulator::arrive(std::mt19937 &rng) {  // NOLINT
    Traffic traffic;
    if (arrival_dist_ == BINOMIAL_PACKET) {
        traffic = generate_binomial_traffic(network_size_, system_clock_,
                                            max_packet_, binom_param_,
                                            min_delay_bound_,
                                            max_delay_bound_, rng);
    } else if (arrival_dist_ == UNIFORM_PACKET) {
        traffic = generate_uniform_traffic(network_size_, system_clock_,
                                           min_packet_, max_packet_,
                                           min_delay_bound_, max_delay_bound_,
                                           rng);
    } else {
        std::cerr << "Error: arrival distribution not recognized!" << std::endl;
        exit(1);
    }
    for (int policy_it = 0; policy_it < POLICY_COUNT; ++policy_it) {
        if (!policy_indicator_[policy_it]) {
            continue;
        }
        for (int bandwidth_it = 0; bandwidth_it < bandwidth_.size();
             ++bandwidth_it) {
            for (int ratio_it = 0; ratio_it < qos_ratio_.size(); ++ratio_it) {
                auto &this_system =
                    queueing_system_[policy_it][bandwidth_it][ratio_it];
                this_system.arrive(traffic, rng);
            }
        }
    }
}

void Simulator::depart(std::mt19937 &rng) {  // NOLINT
    for (int policy_it = 0; policy_it < POLICY_COUNT; ++policy_it) {
        if (!policy_indicator_[policy_it]) {
            continue;
        }
        for (int bandwidth_it = 0; bandwidth_it < bandwidth_.size();
             ++bandwidth_it) {
            for (int ratio_it = 0; ratio_it < qos_ratio_.size(); ++ratio_it) {
                auto &this_system =
                    queueing_system_[policy_it][bandwidth_it][ratio_it];
                this_system.depart(rng);
            }
        }
    }
}

void Simulator::save_deficits() {
    for (int policy_it = 0; policy_it < POLICY_COUNT; ++policy_it) {
        if (!policy_indicator_[policy_it]) {
            continue;
        }
        for (int bandwidth_it = 0; bandwidth_it < bandwidth_.size();
             ++bandwidth_it) {
            for (int ratio_it = 0; ratio_it < qos_ratio_.size(); ++ratio_it) {
                auto &this_system =
                    queueing_system_[policy_it][bandwidth_it][ratio_it];
                this_system.output_deficits(this_system.output_filename());
            }
        }
    }
}

void Simulator::clock_tick() {
    for (int policy_it = 0; policy_it < POLICY_COUNT; ++policy_it) {
        if (!policy_indicator_[policy_it]) {
            continue;
        }
        for (int bandwidth_it = 0; bandwidth_it < bandwidth_.size();
             ++bandwidth_it) {
            for (int ratio_it = 0; ratio_it < qos_ratio_.size(); ++ratio_it) {
                auto &this_system =
                    queueing_system_[policy_it][bandwidth_it][ratio_it];
                this_system.clock_tick();
            }
        }
    }
    ++system_clock_;
}

void Simulator::progress_bar() {
    if (system_clock_ == num_iterations_-1) {
        std::cout << "\rDone!           " << std::endl;
    } else if (system_clock_*HUNDRED/num_iterations_ !=
               (system_clock_-1)*HUNDRED/num_iterations_) {
        std::cout << "\r" << system_clock_*HUNDRED/num_iterations_
                  << "% completed" << std::flush;
    }
}

void Simulator::update_stability_counter() {
    for (int policy_it = 0; policy_it < POLICY_COUNT; ++policy_it) {
        if (!policy_indicator_[policy_it]) {
            continue;
        }
        for (int bandwidth_it = 0; bandwidth_it < bandwidth_.size();
             ++bandwidth_it) {
            for (int ratio_it = 0; ratio_it < qos_ratio_.size(); ++ratio_it) {
                auto &this_system =
                queueing_system_[policy_it][bandwidth_it][ratio_it];
                this_system.update_stability_counter();
            }
        }
    }
}

void Simulator::save_stability_ratios(const std::string &stability_filename) {
    std::ofstream out(stability_filename);
    if (!out) {
        cannot_open_file(stability_filename);
    }
    out << "Bandwidths: " << bandwidth_ << std::endl;
    for (int policy_it = 0; policy_it < POLICY_COUNT; ++policy_it) {
        if (!policy_indicator_[policy_it]) {
            continue;
        }
        EnumParser<Policy> parser_pol;
        out << parser_pol.enum_to_string(static_cast<Policy>(policy_it))
            << ":" << std::endl;
        for (int bandwidth_it = 0; bandwidth_it < bandwidth_.size();
             ++bandwidth_it) {
            for (int ratio_it = 0; ratio_it < qos_ratio_.size(); ++ratio_it) {
                auto &this_system =
                    queueing_system_[policy_it][bandwidth_it][ratio_it];
                out << num_iterations() << " " << qos_ratio_[ratio_it] << " "
                    << this_system.stability_ratio() << std::endl;
            }
        }
    }
    out.close();
}

BooleanVector int_to_bool_vec(int a) {  // TODO(Veggente): generalized case
    BooleanVector indicator(3, false);
    if (a > 3) {
        indicator[2] = true;
    }
    if (a%4 > 1) {
        indicator[1] = true;
    }
    if (a%2 > 0) {
        indicator[0] = true;
    }
    return indicator;
}
