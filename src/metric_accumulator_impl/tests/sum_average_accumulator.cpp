#include "metric_accumulator_impl/sum_average_accumulator.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace analyzer::metric_accumulator::metric_accumulator_impl::test {
namespace {

metric::MetricResult IntegerResult(int value) {
    return {.metric_name = "Integer metric", .value = value};
}

TEST(SumAverageAccumulatorTest, CalculatesSumAndAverage) {
    SumAverageAccumulator accumulator;
    accumulator.Accumulate(IntegerResult(2));
    accumulator.Accumulate(IntegerResult(4));
    accumulator.Finalize();

    EXPECT_EQ(accumulator.Get(), (SumAverageAccumulator::SumAverage{.sum = 6, .average = 3.0}));
}

TEST(SumAverageAccumulatorTest, ResetClearsState) {
    SumAverageAccumulator accumulator;
    accumulator.Accumulate(IntegerResult(2));
    accumulator.Finalize();
    accumulator.Reset();

    EXPECT_THROW(accumulator.Get(), std::runtime_error);
    accumulator.Accumulate(IntegerResult(10));
    accumulator.Finalize();
    EXPECT_EQ(accumulator.Get(), (SumAverageAccumulator::SumAverage{.sum = 10, .average = 10.0}));
}

TEST(SumAverageAccumulatorTest, ReportsInvalidStateAndValueType) {
    SumAverageAccumulator accumulator;
    EXPECT_THROW(accumulator.Get(), std::runtime_error);
    EXPECT_THROW(accumulator.Finalize(), std::runtime_error);
    EXPECT_THROW(accumulator.Accumulate({.metric_name = "String metric", .value = "text"}), std::runtime_error);

    accumulator.Accumulate(IntegerResult(1));
    accumulator.Finalize();
    EXPECT_THROW(accumulator.Accumulate(IntegerResult(2)), std::runtime_error);
}

}  // namespace

}  // namespace analyzer::metric_accumulator::metric_accumulator_impl::test
