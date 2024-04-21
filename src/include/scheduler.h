#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "tools.h"
#include <chrono>
#include <filesystem>
#include <simpleini.h>
#include <string>

/// @brief Period of time that user should spend without the configured device.
/// e.g. 20:00 - 24:00
using BoredPeriod = std::pair<std::chrono::hh_mm_ss<std::chrono::seconds>,
                              std::chrono::hh_mm_ss<std::chrono::seconds>>;

/// @brief USB device with usb_id and name
struct USBDevice
{
    usb_id id;
    std::string name;
    bool operator==(const USBDevice& rhs) const { return this->id == rhs.id; }
};

enum BSchedulerStatus
{
    ENABLED = 0,
    DISABLED = 1,
};

/// @brief Create a chrono object from a string.
/// @param view a string containing hh:mm value.
/// @return the string converted to chrono object.
std::chrono::hh_mm_ss<std::chrono::seconds>
hours_minutes(const std::string_view& view);

/// @brief Parse BoredPeriod from a string.
/// @param view a string containing a time period e.g. 20:00-23:00.
/// @return The BoredPeriod defined by view.
BoredPeriod
parse_bored_period(const std::string_view& view);

/// @brief Parse all BoredPeriods defined in a string.
/// @param view a string containing comma separated BoredPeriods, e.g.
/// 15:00-16:00, 17:00-18:30.
/// @return List of BoredPeriods.
std::vector<BoredPeriod>
parse_bored_periods(const std::string_view& view);

/// @brief Check if time within BoredPeriod.
/// @param now hh_mm_ss containing the current hour, minute and second.
/// @param period BoredPeriod to check.
/// @return True if the time is within the period.
bool
is_in_bored_period(const std::chrono::hh_mm_ss<std::chrono::seconds>& now,
                   const BoredPeriod& period);

/// @brief Check if time point is during a weekend.
/// @param now the time_point to check.
/// @return True if the time point is on a weekend.
bool
is_weekend(const std::chrono::time_point<std::chrono::system_clock>& now);

/// @brief Parse BoredPeriod list and usb_id pairs from a configuration object.
/// @param config a SimpleINI object with the used configuration.
/// @param now the time wanted (current time).
/// @return list of BoredPeriods and USB ids that are valid on the date of now.
std::vector<std::pair<std::vector<BoredPeriod>, USBDevice>>
parse_from_iniconf(const simpleini::SimpleINI& config,
                   const std::chrono::system_clock::time_point& now);

/// @brief Check if the <BoredPeriod, usb_id> list contains an unplugged USB
/// device that should be plugged in.
/// @param bored The <BoredPeriod, USB_device> list to check.
/// @param now_hms current hour, minute and second.
/// @return True if the bored list contains an USB item that should be connected
/// but is not.
bool
has_unconnected(
  const std::vector<std::pair<std::vector<BoredPeriod>, USBDevice>>& bored,
  const std::chrono::hh_mm_ss<std::chrono::seconds>& now_hms);

std::vector<USBDevice>
list_unconnected(
  const std::vector<std::pair<std::vector<BoredPeriod>, USBDevice>>& bored,
  const std::chrono::hh_mm_ss<std::chrono::seconds>& now_hms);

/// @brief Tracks configuration and connected devices
class BoredomScheduler
{
  public:
    BoredomScheduler(){};

    /// @brief Create a BoredomScheduler object with a config.
    /// @param config filepath to a configuration file.
    /// @details The config file should contain configuration items
    /// about devices that should be plugged to the Boredom-lock host device
    /// and the of the times that the device should be plugged in.
    /// The basic configuration should contain a device name as the section,
    /// and each section should contain values usb_id = dead:beef,
    /// weekdays = 0-6, 20-24, weekends = 0-24
    /// The usb_id should contain a VID:PID USB id of the device,
    /// while the weekdays and weekends values contain lists of the times
    /// that the device should be plugged in.
    explicit BoredomScheduler(const std::filesystem::path& config);

    /// @brief Set config file path. init is called after the path is set.
    /// @param config path to config file.
    void set_config_file(const std::filesystem::path& config);

    /// @brief Initialize the object. Reads configuration and saved
    /// enabled/disabled state.
    void init();

    /// @brief Check if alarm needs to be set.
    /// @return true if a device that should be plugged in is not.
    bool is_alarm() const;

    /// @brief Set the alarm off for a time period.
    /// @param seconds Number of seconds to snooze.
    void snooze(std::chrono::seconds seconds);
    void snooze(int seconds) { snooze(std::chrono::seconds(seconds)); };

    /// @brief Disables the alarms until enabled.
    void disable();
    /// @brief Enables the alarms.
    void enable();

    /// @brief Adds a boredom period for device to the current configuration
    /// file.
    /// @param device usb_id of the device.
    /// @param weekday_times string representing weekday bored times, e.g.
    /// 20:00-24:00
    /// @param weekend_times string representing weekend bored times, e.g.
    /// 14:00-24:00
    void create_boredom_period(const USBDevice& device,
                               const std::string& weekday_times,
                               const std::string& weekend_times);

    /// @brief List devices that should be connected but aren't.
    /// @return list of unconnected devices.
    std::vector<USBDevice> list_unconnected_devices() const;

  private:
    /// @brief Configuration file path.
    std::filesystem::path m_configfile;
    /// @brief INI configuration read from m_configfile.
    simpleini::SimpleINI m_config;
    /// @brief File used for saving object status to storage.
    std::filesystem::path m_statusfile;
    std::filesystem::path m_dir;
    BSchedulerStatus m_status;
    std::chrono::seconds m_snooze_t{ 0 };
    std::chrono::time_point<std::chrono::system_clock> m_snooze_start;

    bool m_is_snooze() const;
};

#endif /* SCHEDULER_H */
