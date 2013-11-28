//
//  network_generator.h
//  real-time-scheduler
//
//  Copyright (c) 2013 Xiaohan Kang. All rights reserved.
//

#ifndef REAL_TIME_SCHEDULER_NETWORK_GENERATOR_H_
#define REAL_TIME_SCHEDULER_NETWORK_GENERATOR_H_

#include <vector>
#include <map>

typedef std::vector<bool> BooleanVector;
typedef std::vector<std::vector<bool>> BooleanMatrix;
typedef std::map<int, BooleanMatrix> BooleanMatrixMap;

BooleanMatrix gen_max_matrix_collocated(int network_size);
    // generate maximal schedule matrix for collocated networks

BooleanMatrix gen_max_matrix_cycle(int network_size, int interference_radius);
    // generate maximal schedule matrix for cycle networks

BooleanMatrix gen_max_matrix_line(int network_size, int interference_radius);
    // generate maximal schedule matrix for line networks

BooleanMatrix
gen_max_matrix_line_recursive_with_duplicate_check(int network_size,
    int interference_radius, BooleanMatrixMap& matrix_known);
    // for recursively generating line networks

template <typename Type>
std::vector<std::vector<Type>> append_vector(
    const std::vector<std::vector<Type>>&,
    const std::vector<std::vector<Type>>&);
    // append several vectors to several other vectors

#include "network_generator.hpp"

#endif  // REAL_TIME_SCHEDULER_NETWORK_GENERATOR_H_
