#include "metric_impl/cyclomatic_complexity.hpp"

#include <gtest/gtest.h>

namespace analyzer::metric::metric_impl {
namespace {

int CalculateComplexity(const std::string &filename) {
    const file::File file(filename);
    function::FunctionExtractor extractor;
    const auto functions = extractor.Get(file);
    return std::get<int>(CyclomaticComplexityMetric{}.Calculate(functions.at(0)).value);
}

TEST(CyclomaticComplexityMetricTest, UsesBaseComplexity) {
    EXPECT_EQ(CalculateComplexity("simple.py"), 2);
}

TEST(CyclomaticComplexityMetricTest, CountsIfElifElseAndAssert) {
    EXPECT_EQ(CalculateComplexity("nested_if.py"), 6);
}

TEST(CyclomaticComplexityMetricTest, CountsLoops) {
    EXPECT_EQ(CalculateComplexity("loops.py"), 4);
}

TEST(CyclomaticComplexityMetricTest, CountsExceptionHandling) {
    EXPECT_EQ(CalculateComplexity("exceptions.py"), 5);
}

TEST(CyclomaticComplexityMetricTest, CountsMatchAndCases) {
    EXPECT_EQ(CalculateComplexity("match_case.py"), 5);
}

TEST(CyclomaticComplexityMetricTest, CountsNestedTernaryOperators) {
    EXPECT_EQ(CalculateComplexity("ternary.py"), 3);
}

}  // namespace

}  // namespace analyzer::metric::metric_impl
