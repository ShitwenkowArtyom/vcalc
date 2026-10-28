#pragma once

#include <string>

struct Params {
    std::string authFile = "~/base/vcalc.conf";
    std::string logFile = "~/log/vcalc.log";
    int port = 33333;
};