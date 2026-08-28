#include "metric_accumulator_impl/categorical_accumulator.hpp"

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

void CategoricalAccumulator::Accumulate(const metric::MetricResult &metric_result) {
    if (is_finalized) {
        throw std::runtime_error("CategoricalAccumulator cannot accumulate after Finalize(); call Reset() first");
    }
    const auto *value = std::get_if<std::string>(&metric_result.value);
    if (!value) {
        throw std::runtime_error("CategoricalAccumulator expected a string value for metric '" +
                                 metric_result.metric_name + "'");
    }
    ++categories_freq[*value];
}

void CategoricalAccumulator::Finalize() { is_finalized = true; }

void CategoricalAccumulator::Reset() {
    is_finalized = false;
    categories_freq.clear();
}

const std::unordered_map<std::string, int> &CategoricalAccumulator::Get() const {
    if (!is_finalized)
        throw std::runtime_error("CategoricalAccumulator::Get() called before Finalize()");
    return categories_freq;
}
}  // namespace analyzer::metric_accumulator::metric_accumulator_impl
