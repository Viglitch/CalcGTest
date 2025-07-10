#define GTEST_LINKED_AS_SHARED_LIBRARY 0
#define GMOCK_LINKED_AS_SHARED_LIBRARY 0

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "SimpleCalculator.h"
#include "InMemoryHistory.h"
#include "ICalculator.h"
#include "IHistory.h"
#include <vector>
#include <string>
#include <stdexcept>
#include <climits>

using namespace calc;
using ::testing::_;
using ::testing::Return;

class MockHistory : public IHistory {
public:
    MOCK_METHOD(void, AddEntry, (const std::string& operation), (override));
    MOCK_METHOD(std::vector<std::string>, GetLastOperations, (size_t count), (const, override));
};

class MockCalculator : public ICalculator {
public:
    MOCK_METHOD(int, Add, (int a, int b), (override));
    MOCK_METHOD(int, Subtract, (int a, int b), (override));
    MOCK_METHOD(int, Multiply, (int a, int b), (override));
    MOCK_METHOD(int, Divide, (int a, int b), (override));
    MOCK_METHOD(void, SetHistory, (IHistory& history), (override));
};

class CalculatorTest : public ::testing::Test {
protected:
    InMemoryHistory history;
    SimpleCalculator calculator{ history };
};

TEST_F(CalculatorTest, BasicOperations) {
    EXPECT_EQ(calculator.Add(2, 3), 5);
    EXPECT_EQ(calculator.Subtract(5, 3), 2);
    EXPECT_EQ(calculator.Multiply(2, 3), 6);
    EXPECT_EQ(calculator.Divide(6, 3), 2);
}

TEST_F(CalculatorTest, BoundaryValues) {
    EXPECT_EQ(calculator.Add(INT_MAX, 0), INT_MAX);
    EXPECT_EQ(calculator.Add(INT_MIN, 0), INT_MIN);
    EXPECT_EQ(calculator.Subtract(INT_MAX, INT_MAX), 0);
    EXPECT_EQ(calculator.Multiply(INT_MAX, 1), INT_MAX);
}

TEST_F(CalculatorTest, DivisionByZero) {
    EXPECT_THROW(calculator.Divide(5, 0), std::invalid_argument);
}

TEST_F(CalculatorTest, OperationLogging) {
    calculator.Add(2, 3);
    calculator.Subtract(5, 2);

    auto lastOps = history.GetLastOperations(2);
    ASSERT_EQ(lastOps.size(), 2);
    EXPECT_EQ(lastOps[0], "2 + 3 = 5");
    EXPECT_EQ(lastOps[1], "5 - 2 = 3");
}

TEST(CalculatorMockTest, HistoryInteraction) {
    MockHistory mockHistory;
    SimpleCalculator calculator(mockHistory);

    EXPECT_CALL(mockHistory, AddEntry("2 + 3 = 5")).Times(1);

    calculator.Add(2, 3);
}

TEST(HistoryTest, OperationStorage) {
    InMemoryHistory history;

    history.AddEntry("1 + 1 = 2");
    history.AddEntry("2 * 3 = 6");

    auto lastOps = history.GetLastOperations(1);
    ASSERT_EQ(lastOps.size(), 1);
    EXPECT_EQ(lastOps[0], "2 * 3 = 6");

    lastOps = history.GetLastOperations(5);
    ASSERT_EQ(lastOps.size(), 2);
}

TEST(HistoryMockTest, CalculatorInteraction) {
    InMemoryHistory history;
    MockCalculator mockCalculator;

    EXPECT_CALL(mockCalculator, Add(2, 3)).WillOnce(Return(5));

    mockCalculator.SetHistory(history);
    int result = mockCalculator.Add(2, 3);

    EXPECT_EQ(result, 5);
    auto lastOps = history.GetLastOperations(1);
    ASSERT_TRUE(lastOps.empty());
}