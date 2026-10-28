#include "Calculator.h"

#include <cstdint>
#include <limits>

std::int32_t Calculator::average(const Vector& v) const {
    std::int64_t sum = 0;

    for (std::int32_t x : v) {
        sum += x;

        if (sum > std::numeric_limits<std::int32_t>::max()) {
            return std::numeric_limits<std::int32_t>::max();
        }

        if (sum < std::numeric_limits<std::int32_t>::min()) {
            return std::numeric_limits<std::int32_t>::min();
        }
    }

    return static_cast<std::int32_t>(sum / static_cast<std::int64_t>(v.size()));
}