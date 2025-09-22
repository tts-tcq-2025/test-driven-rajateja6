#include <gtest/gtest.h>
#include "StringCalculator.h"

// Ensures: Empty string returns 0
TEST(StringCalculatorTest, Add_EmptyString_ReturnsZero) {
    EXPECT_EQ(StringCalculator::Add(""), 0);
}