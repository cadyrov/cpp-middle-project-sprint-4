#pragma once
#include <unistd.h>

#include <algorithm>
#include <any>
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
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "metric.hpp"

namespace rv = std::ranges::views;
namespace rs = std::ranges;

namespace analyzer::metric_accumulator {

struct IAccumulator {
    virtual void Accumulate(const metric::MetricResult &metric_result) = 0;
    virtual void Finalize() = 0;
    virtual void Reset() = 0;
    virtual ~IAccumulator() = default;

protected:
    bool is_finalized = false;
};

struct MetricsAccumulator {
    template <typename Accumulator>
    void RegisterAccumulator(const std::string &metric_name, std::unique_ptr<Accumulator> acc) {
        if (!acc) {
            throw std::runtime_error("Cannot register null accumulator for metric '" + metric_name + "'");
        }
        if (accumulators.contains(metric_name)) {
            throw std::runtime_error("Accumulator for metric '" + metric_name + "' is already registered");
        }
        accumulators.emplace(metric_name, std::move(acc));
    }
    template <typename Accumulator>
    const Accumulator &GetFinalizedAccumulator(const std::string &metric_name) const {
        const auto position = accumulators.find(metric_name);
        if (position == accumulators.end()) {
            throw std::runtime_error("Accumulator for metric '" + metric_name + "' is not registered");
        }

        const auto *accumulator = dynamic_cast<const Accumulator *>(position->second.get());
        if (!accumulator) {
            throw std::runtime_error("Accumulator for metric '" + metric_name + "' has an unexpected type");
        }

        position->second->Finalize();
        return *accumulator;
    }
    void AccumulateNextFunctionResults(const std::vector<metric::MetricResult> &metric_results) const;

    void ResetAccumulators();

private:
    std::unordered_map<std::string, std::shared_ptr<IAccumulator>> accumulators;
};

}  // namespace analyzer::metric_accumulator
