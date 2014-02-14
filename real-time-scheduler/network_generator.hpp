//
//  network_generator.hpp
//  real-time-scheduler
//
//  Copyright (c) 2013-2014 Xiaohan Kang. All rights reserved.
//

#ifndef REAL_TIME_SCHEDULER_NETWORK_GENERATOR_HPP_
#define REAL_TIME_SCHEDULER_NETWORK_GENERATOR_HPP_

#include <set>

template <typename Type>
std::vector<std::vector<Type>> append_vector(
    const std::vector<std::vector<Type>>& a,
    const std::vector<std::vector<Type>>& b) {
    std::vector<std::vector<Type>> M;
    if (a.empty()) {  // 0+b = b
        M = b;
    } else if (b.empty()) {  // a+0 = a
        M = a;
    } else {
        for (int i = 0; i < a.size(); ++i) {
            for (int j = 0; j < b.size(); ++j) {
                M.push_back(a[i]);
                M[i*b.size()+j].insert(M[i*b.size()+j].end(), b[j].begin(),
                                       b[j].end());
            }
        }
    }
    return M;
}

template <typename Type>
std::vector<Type> eliminate_duplicates(const std::vector<Type>& v) {
    if (v.size() <= 1) {
        return v;
    } else {
        std::set<Type> s(v.begin(), v.end());
        std::vector<Type> uniq(s.begin(), s.end());  // NOLINT
        return uniq;
    }
}

#endif  // REAL_TIME_SCHEDULER_NETWORK_GENERATOR_HPP_
