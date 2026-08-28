#include "metric_impl/parameters_count.hpp"

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

namespace analyzer::metric::metric_impl {
std::string CountParametersMetric::Name() const { return kName; }

MetricResult::ValueType CountParametersMetric::CalculateImpl(const function::Function &f) const {
    auto &function_ast = f.ast;
    // 1. Находим начало блока параметров
    const std::string parameters_marker = "(parameters";
    size_t params_start = function_ast.find(parameters_marker);
    if (params_start == std::string::npos) {
        return 0;
    }

    // Каждый непосредственный дочерний узел `parameters` описывает один параметр.
    // Вложенные identifier (например, внутри default_parameter) отдельно не считаются.
    int count = 0;
    int depth = 1;
    const std::string_view parameters_body(function_ast.data() + params_start + parameters_marker.size(),
                                           function_ast.size() - params_start - parameters_marker.size());
    [[maybe_unused]] const auto parameters_end = std::ranges::find_if(parameters_body, [&](char symbol) {
        if (symbol == '(') {
            if (depth == 1) {
                ++count;
            }
            ++depth;
        } else if (symbol == ')') {
            --depth;
        }
        return depth == 0;
    });

    return count;
}

}  // namespace analyzer::metric::metric_impl
