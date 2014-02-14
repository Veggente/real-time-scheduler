//
//  network_generator.cpp
//  real-time-scheduler
//
//  Copyright (c) 2013-2014 Xiaohan Kang. All rights reserved.
//

#include "./network_generator.h"
#include <map>
#include <fstream>  // NOLINT
#include <sstream>
#include <string>
#include <limits>

BooleanMatrix gen_max_matrix_collocated(int network_size) {
    BooleanMatrix maximal_schedule_matrix;
    for (int i = 0; i < network_size; ++i) {
        BooleanVector schedule(network_size, false);
        schedule[i] = true;
        maximal_schedule_matrix.push_back(schedule);
    }
    return maximal_schedule_matrix;
}

BooleanMatrix gen_max_matrix_cycle(int network_size, int interference_radius) {
    if (network_size <= 2*interference_radius+1) {
        return gen_max_matrix_collocated(network_size);
    }  // else, proceed
    BooleanMatrix maximal_schedule_matrix;
    BooleanMatrixMap maximal_schedule_matrix_map;  // for duplication check
    for (int i = 0; i <= interference_radius; ++i) {
            // false*i+true*1+false*interference_radius at the head
        BooleanVector maximal_head(i, false);
        maximal_head.push_back(true);
        maximal_head.insert(maximal_head.end(), interference_radius, false);
        BooleanVector maximal_tail(interference_radius-i, false);
        BooleanMatrix schedules_head(1, maximal_head);
        BooleanMatrix schedules_tail(1, maximal_tail);
        BooleanMatrix matrix_append = append_vector(
            schedules_head, append_vector(
            gen_max_matrix_line_recursive_with_duplicate_check(network_size-2*
            interference_radius-1, interference_radius,
            maximal_schedule_matrix_map), schedules_tail));
        maximal_schedule_matrix.insert(maximal_schedule_matrix.end(),
            matrix_append.begin(), matrix_append.end());
    }
    for (int i = 0; i < interference_radius; ++i) {
            // false*interference_radius+true*1+false*i at the tail
        BooleanVector maximal_head(interference_radius-i, false);
        BooleanVector maximal_tail(interference_radius, false);
        maximal_tail.push_back(true);
        maximal_tail.insert(maximal_tail.end(), i, false);
        BooleanMatrix schedules_head(1, maximal_head);
        BooleanMatrix schedules_tail(1, maximal_tail);
        BooleanMatrix matrix_append = append_vector(
            schedules_head, append_vector(
            gen_max_matrix_line_recursive_with_duplicate_check(network_size-2*
            interference_radius-1, interference_radius,
            maximal_schedule_matrix_map), schedules_tail));
        maximal_schedule_matrix.insert(maximal_schedule_matrix.end(),
            matrix_append.begin(), matrix_append.end());
    }
    maximal_schedule_matrix = eliminate_duplicates(maximal_schedule_matrix);
        // eliminate the duplicates
    return maximal_schedule_matrix;
}

BooleanMatrix gen_max_matrix_line(int network_size,
                                                   int interference_radius) {
    BooleanMatrix maximal_schedule_matrix;
    BooleanMatrixMap maximal_schedule_matrix_map;
    maximal_schedule_matrix = gen_max_matrix_line_recursive_with_duplicate_check
        (network_size, interference_radius, maximal_schedule_matrix_map);
    return maximal_schedule_matrix;
}

BooleanMatrix
    gen_max_matrix_line_recursive_with_duplicate_check(int network_size,
    int interference_radius, BooleanMatrixMap &matrix_known) {
        // matrix_known has keys of known lengths
    if (network_size <= interference_radius+1) {  // collocated
        BooleanMatrix maximal_matrix =
            gen_max_matrix_collocated(network_size);
        matrix_known[network_size] = maximal_matrix;
        return maximal_matrix;
    } else if (matrix_known.count(network_size) > 0) {
            // network_size-line network is already known
        return matrix_known.find(network_size)->second;
    } else {  // network_size-line has not been calculated
        BooleanMatrix maximal_matrix;
        for (int i = 0; i <= interference_radius ; ++i) {
            BooleanVector maximal(i, false);
            maximal.push_back(true);
            if (i+1+interference_radius < network_size) {
                maximal.insert(maximal.end(), interference_radius, false);
                BooleanMatrix maximal_head(1, maximal);
                BooleanMatrix maximal_temp =
                    append_vector(maximal_head,
                    gen_max_matrix_line_recursive_with_duplicate_check(
                    network_size-i-1-interference_radius,
                    interference_radius, matrix_known));
                        // recursive step
                maximal_matrix.insert(maximal_matrix.end(),
                                      maximal_temp.begin(), maximal_temp.end());
            } else {  // no recursion needed
                maximal.insert(maximal.end(), network_size-i-1, false);
                maximal_matrix.push_back(maximal);
            }
        }
        matrix_known[network_size] = maximal_matrix;
        return maximal_matrix;
    }
}

BooleanMatrix load_network(const std::string network_filename) {
    BooleanMatrix maximal_schedule_matrix;
    int network_size;
    int num_maximal_schedules;
    std::ifstream in(network_filename);
    if (!in) {
        cannot_open_file(network_filename);
    }
    in.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    in >> network_size;
    in.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    in >> num_maximal_schedules;
    maximal_schedule_matrix = BooleanMatrix(num_maximal_schedules,
                                            BooleanVector(network_size, false));
    in.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    std::string max_schedule_str;
    std::getline(in, max_schedule_str);  // to omit the first empty line
    for (int i = 0; i < num_maximal_schedules; ++i) {
        std::getline(in, max_schedule_str);
        std::istringstream max_schedule_stream(max_schedule_str);
        std::string word;
        while (max_schedule_stream >> word) {
            maximal_schedule_matrix[i][std::stoi(word)-1] = true;
        }
    }
    return maximal_schedule_matrix;
}
