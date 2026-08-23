#include "metric_impl/code_lines_count.hpp"

#include <unistd.h>

#include <algorithm>
#include <array>
#include <charconv>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <ranges>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace analyzer::metric::metric_impl {
std::string CodeLinesCountMetric::Name() const { return kName; }

MetricResult::ValueType CodeLinesCountMetric::CalculateImpl(const function::Function &f) const {
    const auto &function_ast = f.ast;

    // Вспомогательная лямбда для извлечения номера строки из диапазона узла AST.
    // Формат узла в S-выражении: (node_type [start_line,start_column] [end_line,end_column] ...)
    // Эта функция ищет открывающую скобку "[" после заданной позиции и парсит первую координату — номер строки.
    auto line_number = [&](std::size_t start_pos) -> std::optional<int> {
        const std::size_t line_pos = function_ast.find('[', start_pos);
        if (line_pos == std::string::npos) {
            return std::nullopt;
        }

        const std::size_t comma_pos = function_ast.find(',', line_pos);
        if (comma_pos == std::string::npos) {
            return std::nullopt;
        }

        int result = 0;
        const char *first = function_ast.data() + line_pos + 1;
        const char *last = function_ast.data() + comma_pos;
        const auto [parsed_to, error] = std::from_chars(first, last, result);
        if (error != std::errc{} || parsed_to != last) {
            return std::nullopt;
        }
        return result;
    };
    // Определяем начальную и конечную строки тела функции:
    // - начальная строка берётся из корневого узла функции (первое вхождение "[")
    // - конечная строка ищется по шаблону "] -"
    const auto start_line = line_number(0);
    const std::size_t range_separator = function_ast.find("] -");
    const auto end_line = range_separator == std::string::npos ? std::nullopt : line_number(range_separator);
    if (!start_line || !end_line || *end_line <= *start_line) {
        return 0;
    }

    // Лямбда, проверяющая, является ли конкретная строка "кодовой", то есть не комментарием.
    auto is_code_line = [&](int line) {
        const std::string line_marker = "[" + std::to_string(line) + ",";
        std::size_t line_pos = 0;

        while ((line_pos = function_ast.find(line_marker, line_pos)) != std::string::npos) {
            const std::size_t node_start = function_ast.rfind('(', line_pos);
            if (node_start != std::string::npos) {
                const std::size_t node_end = function_ast.find_first_of(" \n[", node_start + 1);
                if (node_end == std::string::npos) {
                    return false;
                }
                const std::string_view node_type(function_ast.data() + node_start + 1,
                                                 node_end - node_start - 1);
                if (node_type != "comment") {
                    return true;
                }
            }
            line_pos += line_marker.size();
        }

        return false;
    };
    // === ВАШ КОД ДОЛЖЕН БЫТЬ ЗДЕСЬ ===
    //
    // Цель: подсчитать количество строк в диапазоне [start_line + 1, end_line],
    // которые действительно содержат код (а не только комментарии или пустые строки).
    //
    // Почему start_line + 1?
    // Потому что первая строка — это строка с объявлением функции (def ...),
    // а тело функции начинается со следующей строки (обычно с отступа).
    int count = 0;
    for (int line = *start_line + 1; line <= *end_line; ++line) {
        count += is_code_line(line);
    }
    return count;
}

}  // namespace analyzer::metric::metric_impl
