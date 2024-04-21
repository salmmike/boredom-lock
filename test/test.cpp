
#include <cassert>
#include <gtest/gtest.h>
#include <iostream>
#include <scheduler.h>

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

#define TEST_FILE_PATH "/tmp/boredomlock-test.ini"
void
create_test_file(usb_id device,
                 const std::string& weekday,
                 const std::string& weekend)
{
    std::ofstream testconfig(TEST_FILE_PATH);
    testconfig << "[Test entry]\nusb_id = " << device.to_string()
               << "\nweekdays = " << weekday << "\nweekend=" << weekend << "\n";
    testconfig.close();
}

TEST(NAME, test_boredom_scheduler_is_alarm)
{
    usb_id id;
    id.vid = 0xdead;
    id.pid = 0xbeef;

    create_test_file(id, "00:00-24:00", "00:00-24:00");
    auto sched = BoredomScheduler{ TEST_FILE_PATH };
    sched.init();
    ASSERT_TRUE(sched.is_alarm());
    sched.snooze(std::chrono::seconds(1));
    ASSERT_FALSE(sched.is_alarm());
    usleep(1200000);
    ASSERT_TRUE(sched.is_alarm());
    sched.disable();
    ASSERT_FALSE(sched.is_alarm());
    sched.enable();
}

TEST(NAME, test_boredom_scheduler_no_alarm)
{
    usb_id id;
    id.vid = 0xdead;
    id.pid = 0xbeef;

    create_test_file(id, "00:00-00:00", "00:00-00:00");
    auto sched = BoredomScheduler{ TEST_FILE_PATH };
    sched.init();
    ASSERT_FALSE(sched.is_alarm());
}

int
main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
