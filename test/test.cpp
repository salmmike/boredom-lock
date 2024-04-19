
#include "scheduler.h"
#include <cassert>
#include <gtest/gtest.h>
#include <iostream>

#define NAME scheduler_test

TEST(NAME, test_hours_minutes)
{
    auto value = hours_minutes("08:39");
    ASSERT_EQ(value.seconds().count(),
              std::chrono::hh_mm_ss<std::chrono::seconds>{
                std::chrono::hours(8) + std::chrono::minutes(39) }
                .seconds()
                .count());
}

TEST(NAME, test_parse_bored_period)
{
    auto value = parse_bored_period("08:39-09:00");

    ASSERT_EQ(value.first.seconds().count(),
              std::chrono::hh_mm_ss<std::chrono::seconds>{
                std::chrono::hours(8) + std::chrono::minutes(39) }
                .seconds()
                .count());

    ASSERT_EQ(value.second.seconds().count(),
              std::chrono::hh_mm_ss<std::chrono::seconds>{
                std::chrono::hours(0) + std::chrono::minutes(0) }
                .seconds()
                .count());
}

TEST(NAME, test_parse_bored_periods)
{
    auto values = parse_bored_periods("04:30-5:45, 06:15-07:30, 08:39-09:00");

    ASSERT_EQ(values[2].first.hours().count(),
              std::chrono::hh_mm_ss<std::chrono::seconds>{
                std::chrono::hours(8) + std::chrono::minutes(39) }
                .hours()
                .count());

    ASSERT_EQ(
      values[0].second.minutes().count(),
      std::chrono::hh_mm_ss<std::chrono::seconds>{ std::chrono::minutes(45) }
        .minutes()
        .count());
}

TEST(NAME, test_in_bored_period)
{
    auto values = parse_bored_periods("04:30-5:45, 06:15-07:30, 08:39-09:00 ");

    auto in_period_1 =
      std::chrono::hh_mm_ss<std::chrono::seconds>{ std::chrono::hours(4) +
                                                   std::chrono::minutes(45) };
    auto in_period_2 =
      std::chrono::hh_mm_ss<std::chrono::seconds>{ std::chrono::hours(6) +
                                                   std::chrono::minutes(45) };

    auto in_period_3 =
      std::chrono::hh_mm_ss<std::chrono::seconds>{ std::chrono::hours(8) +
                                                   std::chrono::minutes(45) };

    ASSERT_TRUE(is_in_bored_period(in_period_1, values[0]));
    ASSERT_TRUE(is_in_bored_period(in_period_2, values[1]));
    ASSERT_TRUE(is_in_bored_period(in_period_3, values[2]));

    ASSERT_FALSE(is_in_bored_period(in_period_2, values[0]));
    ASSERT_FALSE(is_in_bored_period(in_period_3, values[0]));

    ASSERT_FALSE(is_in_bored_period(in_period_1, values[1]));
    ASSERT_FALSE(is_in_bored_period(in_period_3, values[1]));

    ASSERT_FALSE(is_in_bored_period(in_period_1, values[2]));
    ASSERT_FALSE(is_in_bored_period(in_period_2, values[2]));
}

int
main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
