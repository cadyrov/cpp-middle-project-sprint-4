#include "metric_impl/parameters_count.hpp"

#include <gtest/gtest.h>

namespace analyzer::metric::metric_impl {
namespace {

int CalculateParametersCount(const std::string &filename) {
    const file::File file(filename);
    function::FunctionExtractor extractor;
    const auto functions = extractor.Get(file);
    return std::get<int>(CountParametersMetric{}.Calculate(functions.at(0)).value);
}

TEST(CountParametersMetricTest, CountsFunctionWithoutParameters) {
    EXPECT_EQ(CalculateParametersCount("simple.py"), 0);
}

TEST(CountParametersMetricTest, CountsRegularParameters) {
    EXPECT_EQ(CalculateParametersCount("comments.py"), 3);
}

TEST(CountParametersMetricTest, CountsDefaultAndVariadicParameters) {
    EXPECT_EQ(CalculateParametersCount("many_parameters.py"), 5);
}

}  // namespace

}  // namespace analyzer::metric::metric_impl
