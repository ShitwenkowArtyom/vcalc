#pragma once

#include <cstdint>
#include <vector>

typedef std::vector<std::int32_t> Vector;

class Calculator {
public:
    std::int32_t average(const Vector& v) const;
};