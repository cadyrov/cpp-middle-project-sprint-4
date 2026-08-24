#include "metric_accumulator_impl/average_accumulator.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace analyzer::metric_accumulator::metric_accumulator_impl::test {
namespace {

metric::MetricResult IntegerResult(int value) {
    return {.metric_name = "Integer metric", .value = value};
}

TEST(AverageAccumulatorTest, CalculatesAverage) {
    AverageAccumulator accumulator;
    accumulator.Accumulate(IntegerResult(2));
    accumulator.Accumulate(IntegerResult(4));
    accumulator.Finalize();

    EXPECT_DOUBLE_EQ(accumulator.Get(), 3.0);
}

TEST(AverageAccumulatorTest, ResetClearsState) {
    AverageAccumulator accumulator;
    accumulator.Accumulate(IntegerResult(2));
    accumulator.Finalize();
    accumulator.Reset();

    EXPECT_THROW(accumulator.Get(), std::runtime_error);
    accumulator.Accumulate(IntegerResult(10));
    accumulator.Finalize();
    EXPECT_DOUBLE_EQ(accumulator.Get(), 10.0);
}

TEST(AverageAccumulatorTest, ReportsInvalidStateAndValueType) {
    AverageAccumulator accumulator;
    EXPECT_THROW(accumulator.Get(), std::runtime_error);
    EXPECT_THROW(accumulator.Finalize(), std::runtime_error);
    EXPECT_THROW(accumulator.Accumulate({.metric_name = "String metric", .value = "text"}), std::runtime_error);

    accumulator.Accumulate(IntegerResult(1));
    accumulator.Finalize();
    EXPECT_THROW(accumulator.Accumulate(IntegerResult(2)), std::runtime_error);
}

TEST(MetricsAccumulatorTest, AggregatesRegisteredMetric) {
    MetricsAccumulator accumulator;
    accumulator.RegisterAccumulator("Integer metric", std::make_unique<AverageAccumulator>());
    accumulator.AccumulateNextFunctionResults({IntegerResult(2)});
    accumulator.AccumulateNextFunctionResults({IntegerResult(4)});

    const auto &average = accumulator.GetFinalizedAccumulator<AverageAccumulator>("Integer metric");
    EXPECT_DOUBLE_EQ(average.Get(), 3.0);
}

TEST(MetricsAccumulatorTest, ReportsRegistrationAndLookupErrors) {
    MetricsAccumulator accumulator;
    EXPECT_THROW(accumulator.RegisterAccumulator("Null", std::unique_ptr<AverageAccumulator>{}), std::runtime_error);

    accumulator.RegisterAccumulator("Integer metric", std::make_unique<AverageAccumulator>());
    EXPECT_THROW(accumulator.RegisterAccumulator("Integer metric", std::make_unique<AverageAccumulator>()),
                 std::runtime_error);
    EXPECT_THROW(accumulator.AccumulateNextFunctionResults({IntegerResult(1),
                                                            {.metric_name = "Unknown", .value = 2}}),
                 std::runtime_error);
    EXPECT_THROW(accumulator.GetFinalizedAccumulator<AverageAccumulator>("Unknown"), std::runtime_error);
}

}  // namespace

}  // namespace analyzer::metric_accumulator::metric_accumulator_impl::test
