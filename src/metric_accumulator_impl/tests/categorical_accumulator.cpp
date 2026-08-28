

#include "metric_accumulator_impl/categorical_accumulator.hpp"

#include <gtest/gtest.h>

#include <stdexcept>

namespace analyzer::metric_accumulator::metric_accumulator_impl::test {
namespace {

metric::MetricResult CategoryResult(std::string value) {
    return {.metric_name = "Category metric", .value = std::move(value)};
}

TEST(CategoricalAccumulatorTest, CountsCategories) {
    CategoricalAccumulator accumulator;
    accumulator.Accumulate(CategoryResult("Snake Case"));
    accumulator.Accumulate(CategoryResult("Camel Case"));
    accumulator.Accumulate(CategoryResult("Snake Case"));
    accumulator.Finalize();

    const auto &frequencies = accumulator.Get();
    EXPECT_EQ(frequencies.at("Snake Case"), 2);
    EXPECT_EQ(frequencies.at("Camel Case"), 1);
}

TEST(CategoricalAccumulatorTest, ResetClearsState) {
    CategoricalAccumulator accumulator;
    accumulator.Accumulate(CategoryResult("Snake Case"));
    accumulator.Finalize();
    accumulator.Reset();

    EXPECT_THROW(accumulator.Get(), std::runtime_error);
    accumulator.Finalize();
    EXPECT_TRUE(accumulator.Get().empty());
}

TEST(CategoricalAccumulatorTest, ReportsInvalidStateAndValueType) {
    CategoricalAccumulator accumulator;
    EXPECT_THROW(accumulator.Get(), std::runtime_error);
    EXPECT_THROW(accumulator.Accumulate({.metric_name = "Integer metric", .value = 1}), std::runtime_error);

    accumulator.Accumulate(CategoryResult("Snake Case"));
    accumulator.Finalize();
    EXPECT_THROW(accumulator.Accumulate(CategoryResult("Camel Case")), std::runtime_error);
}

}  // namespace

}  // namespace analyzer::metric_accumulator::metric_accumulator_impl::test
