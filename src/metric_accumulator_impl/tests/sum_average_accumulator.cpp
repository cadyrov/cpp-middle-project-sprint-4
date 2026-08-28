#include "metric_accumulator_impl/sum_average_accumulator.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace analyzer::metric_accumulator::metric_accumulator_impl::test { namespace {

constexpr double kEpsilon = 1e-6;

metric::MetricResult IntegerResult(int value) { return {.metric_name = "Integer metric", .value = value}; }

TEST(SumAverageAccumulatorTest, CalculatesSumAndAverage) {
    SumAverageAccumulator accumulator;
    accumulator.Accumulate(IntegerResult(2));
    accumulator.Accumulate(IntegerResult(4));
    accumulator.Finalize();

    EXPECT_NEAR(accumulator.Get().sum, 6, kEpsilon);
    EXPECT_NEAR(accumulator.Get().average, 3.0, kEpsilon);
}

TEST(SumAverageAccumulatorTest, ResetClearsState) {
    SumAverageAccumulator accumulator;
    accumulator.Accumulate(IntegerResult(2));
    accumulator.Finalize();
    accumulator.Reset();

    EXPECT_THROW(accumulator.Get(), std::runtime_error);
    accumulator.Accumulate(IntegerResult(10));
    accumulator.Finalize();
    EXPECT_NEAR(accumulator.Get().sum, 10, kEpsilon);
    EXPECT_NEAR(accumulator.Get().average, 10.0, kEpsilon);
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

}}  // namespace analyzer::metric_accumulator::metric_accumulator_impl::test
