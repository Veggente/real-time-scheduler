//
//  network_generator.h
//  real-time-scheduler
//
//  Copyright (c) 2013-2014 Xiaohan Kang. All rights reserved.
//

#ifndef REAL_TIME_SCHEDULER_NETWORK_GENERATOR_H_
#define REAL_TIME_SCHEDULER_NETWORK_GENERATOR_H_

#include <vector>
#include <string>
#include "./common.h"

BooleanMatrix gen_max_matrix_collocated(int network_size);
    // Generate maximal schedule matrix for collocated networks.

BooleanMatrix gen_max_matrix_cycle(int network_size, int interference_radius);
    // Generate maximal schedule matrix for cycle networks.

BooleanMatrix gen_max_matrix_line(int network_size, int interference_radius);
    // Generate maximal schedule matrix for line networks.

BooleanMatrix
gen_max_matrix_line_recursive_with_duplicate_check(int network_size,
    int interference_radius, BooleanMatrixMap& matrix_known);
    // For recursively generating line networks.

BooleanMatrix load_network(const std::string network_filename);
    // Load network from file.

template <typename Type>
std::vector<std::vector<Type>> append_vector(
    const std::vector<std::vector<Type>>&,
    const std::vector<std::vector<Type>>&);
    // Append several vectors to several other vectors.

#include "network_generator.hpp"

#endif  // REAL_TIME_SCHEDULER_NETWORK_GENERATOR_H_
