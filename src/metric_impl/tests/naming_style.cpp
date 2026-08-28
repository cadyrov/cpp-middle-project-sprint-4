#include "metric_impl/naming_style.hpp"

#include <gtest/gtest.h>

namespace analyzer::metric::metric_impl {
namespace {

std::string CalculateNamingStyle(const std::string &filename) {
    const file::File file(filename);
    function::FunctionExtractor extractor;
    const auto functions = extractor.Get(file);
    return std::get<std::string>(NamingStyleMetric{}.Calculate(functions.at(0)).value);
}

TEST(NamingStyleMetricTest, DetectsSnakeCase) {
    EXPECT_EQ(CalculateNamingStyle("simple.py"), "Snake Case");
}

TEST(NamingStyleMetricTest, DetectsCamelCase) {
    EXPECT_EQ(CalculateNamingStyle("if.py"), "Camel Case");
}

TEST(NamingStyleMetricTest, DetectsPascalCase) {
    EXPECT_EQ(CalculateNamingStyle("loops.py"), "Pascal Case");
}

}  // namespace

}  // namespace analyzer::metric::metric_impl
