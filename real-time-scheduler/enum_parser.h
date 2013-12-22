//
//  enum_parser.h
//  real-time-scheduler
//
//  Copyright (c) 2013 Xiaohan Kang. All rights reserved.
//

#ifndef REAL_TIME_SCHEDULER_ENUM_PARSER_H_
#define REAL_TIME_SCHEDULER_ENUM_PARSER_H_

#include <string>
#include <iostream>  // NOLINT
#include <map>
#include "./common.h"

template <typename ENUM>
class EnumParser {
public:  // NOLINT
    EnumParser() {}
    std::string enum_to_string(ENUM e);
    ENUM string_to_enum(const std::string &s);
private:  // NOLINT
    std::map<std::string, ENUM> string_to_enum_;
    std::map<ENUM, std::string> enum_to_string_;
};

template <typename ENUM>
std::string EnumParser<ENUM>::enum_to_string(ENUM e) {
    typename std::map<ENUM, std::string>::const_iterator it =
        enum_to_string_.find(e);
    if (it == enum_to_string_.end()) {
        std::cerr << "Error parsing enum!" << std::endl;
        exit(1);
    }
    return it->second;
}

template <typename ENUM>
ENUM EnumParser<ENUM>::string_to_enum(const std::string &s) {
    typename std::map<std::string, ENUM>::const_iterator it =
        string_to_enum_.find(s);
    if (it == string_to_enum_.end()) {
        std::cerr << "Error parsing enum!" << std::endl;
        exit(1);
    }
    return it->second;
}

template<>
EnumParser<Policy>::EnumParser() {
    enum_to_string_[LDF] = "ldf";
    enum_to_string_[EDF] = "edf";
    enum_to_string_[SDBF] = "sdbf";
    string_to_enum_["ldf"] = LDF;
    string_to_enum_["edf"] = EDF;
    string_to_enum_["sdbf"] = SDBF;
}

template<>
EnumParser<NetworkType>::EnumParser() {
    enum_to_string_[COLLOCATED] = "collocated";
    enum_to_string_[LINE] = "line";
    enum_to_string_[CYCLE] = "cycle";
    enum_to_string_[UNIT_DISK] = "unit-disk";
    string_to_enum_["collocated"] = COLLOCATED;
    string_to_enum_["line"] = LINE;
    string_to_enum_["cycle"] = CYCLE;
    string_to_enum_["unit-disk"] = UNIT_DISK;
}

template<>
EnumParser<ArrivalDistribution>::EnumParser() {
    enum_to_string_[UNIFORM_PACKET] = "uniform";
    enum_to_string_[BINOMIAL_PACKET] = "binomial";
    string_to_enum_["uniform"] = UNIFORM_PACKET;
    string_to_enum_["binomial"] = BINOMIAL_PACKET;
}

#endif  // REAL_TIME_SCHEDULER_ENUM_PARSER_H_
