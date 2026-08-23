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
    std::size_t position = params_start + parameters_marker.size();
    while (position < function_ast.size() && depth > 0) {
        if (function_ast[position] == '(') {
            if (depth == 1) {
                ++count;
            }
            ++depth;
        } else if (function_ast[position] == ')') {
            --depth;
        }
        ++position;
    }

    return count;
}

}  // namespace analyzer::metric::metric_impl
