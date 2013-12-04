//
//  main.cpp
//  real-time-scheduler
//
//  Copyright (c) 2013 Xiaohan Kang. All rights reserved.
//

#include <string>
#include <fstream>  // NOLINT
#include "./simulator.h"

int main(int argc, const char * argv[]) {
    std::string input_file(argv[1]);
    return simulator(input_file);
}
