#include "metric_impl/code_lines_count.hpp"

#include <gtest/gtest.h>

namespace analyzer::metric::metric_impl {
namespace {

int CalculateCodeLines(const std::string &filename) {
    const file::File file(filename);
    function::FunctionExtractor extractor;
    const auto functions = extractor.Get(file);
    return std::get<int>(CodeLinesCountMetric{}.Calculate(functions.at(0)).value);
}

TEST(CodeLinesCountMetricTest, CountsSimpleFunctionBody) {
    EXPECT_EQ(CalculateCodeLines("simple.py"), 5);
}

TEST(CodeLinesCountMetricTest, IgnoresComments) {
    EXPECT_EQ(CalculateCodeLines("comments.py"), 3);
}

TEST(CodeLinesCountMetricTest, IgnoresBlankLinesAndCountsMultilineCode) {
    EXPECT_EQ(CalculateCodeLines("many_lines.py"), 11);
}

}  // namespace

}  // namespace analyzer::metric::metric_impl
