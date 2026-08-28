#include "metric_accumulator_impl/average_accumulator.hpp"

#include <unistd.h>

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace analyzer::metric_accumulator::metric_accumulator_impl {

void AverageAccumulator::Accumulate(const metric::MetricResult &metric_result) {
    if (is_finalized) {
        throw std::runtime_error("AverageAccumulator cannot accumulate after Finalize(); call Reset() first");
    }
    const auto *value = std::get_if<int>(&metric_result.value);
    if (!value) {
        throw std::runtime_error("AverageAccumulator expected an integer value for metric '" +
                                 metric_result.metric_name + "'");
    }
    sum += *value;
    ++count;
}
void AverageAccumulator::Finalize() {
    if (count == 0) {
        throw std::runtime_error("AverageAccumulator cannot finalize without accumulated values");
    }
    average = static_cast<double>(sum) / count;
    is_finalized = true;
}

void AverageAccumulator::Reset() {
    is_finalized = false;
    sum = 0;
    count = 0;
    average = 0;
}

double AverageAccumulator::Get() const {
    if (!is_finalized)
        throw std::runtime_error("AverageAccumulator::Get() called before Finalize()");
    return average;
}
}  // namespace analyzer::metric_accumulator::metric_accumulator_impl
