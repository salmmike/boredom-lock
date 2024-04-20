#include "scheduler.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <unistd.h>

std::chrono::hh_mm_ss<std::chrono::seconds>
hours_minutes(const std::string_view& view)
{
    short hour = 0;
    short min = 0;
    std::stringstream ss;

    auto delimiter = view.find(':');

    auto start_str = view.substr(0, delimiter);
    auto end_str = view.substr(delimiter + 1);

    ss << start_str;
    ss >> hour;
    if (delimiter != std::string::npos) {
        ss.clear();
        ss << end_str;
        ss >> min;
    }

    return std::chrono::hh_mm_ss<std::chrono::seconds>{
        std::chrono::hours(hour) + std::chrono::minutes(min)
    };
}

BoredPeriod
parse_bored_period(const std::string_view& view)
{
    auto delimiter = view.find('-');

    auto start = hours_minutes(view.substr(0, delimiter));
    auto end = hours_minutes(view.substr(delimiter + 1));

    return BoredPeriod{ start, end };
}

std::vector<BoredPeriod>
parse_bored_periods(const std::string_view& view)
{
    std::vector<BoredPeriod> periods{};
    size_t delimiter = 0;
    size_t prev = 0;

    while (delimiter < std::string::npos) {
        delimiter = view.find(',', prev);
        periods.push_back(parse_bored_period(view.substr(prev, delimiter)));
        prev = delimiter + 1;
    }

    return periods;
}

bool
is_in_bored_period(const std::chrono::hh_mm_ss<std::chrono::seconds>& now,
                   const BoredPeriod& period)
{
    return ((now.to_duration() >= period.first.to_duration()) &&
            now.to_duration() <= period.second.to_duration());
}

bool
is_in_bored_periods(const std::chrono::hh_mm_ss<std::chrono::seconds>& now,
                    const std::vector<BoredPeriod>& periods)
{
    return std::any_of(std::begin(periods), std::end(periods), [now](auto prd) {
        return is_in_bored_period(now, prd);
    });
}

bool
is_weekend(const std::chrono::time_point<std::chrono::system_clock>& now)
{
    const auto now_time_t = std::chrono::system_clock::to_time_t(now);
    const auto local_tm = std::localtime(&now_time_t);

    return (local_tm->tm_wday == std::chrono::Saturday.c_encoding() ||
            local_tm->tm_wday == std::chrono::Sunday.c_encoding());
}

std::vector<std::pair<std::vector<BoredPeriod>, usb_id>>
parse_from_iniconf(const simpleini::SimpleINI& config,
                   const std::chrono::system_clock::time_point& now)
{

    const auto key = is_weekend(now) ? "weekend" : "weekdays";
    const auto map = config.get_map();
    std::vector<std::pair<std::vector<BoredPeriod>, usb_id>> bored{
        map.size()
    };

    std::transform(
      std::begin(map), std::end(map), std::begin(bored), [key](auto item) {
          return std::pair<std::vector<BoredPeriod>, usb_id>{
              parse_bored_periods(item.second.get(key)),
              usb_id_from_string(item.second.get("usb_id"))
          };
      });

    return bored;
}

bool
has_unconnected(
  const std::vector<std::pair<std::vector<BoredPeriod>, usb_id>>& bored,
  const std::chrono::hh_mm_ss<std::chrono::seconds>& now_hms)
{
    for (const auto& item : bored) {
        if (is_in_bored_periods(now_hms, item.first)) {
            if (!usb_id_is_connected(item.second)) {
                return true;
            }
        }
    }
    return false;
}

BoredomScheduler::BoredomScheduler(const std::filesystem::path& config)
  : m_configfile(config)
{
    const char* homedir = getenv("HOME");
    m_dir = std::filesystem::path(homedir) /
            std::filesystem::path(".local/share/BoredomScheduler/");
    m_statusfile = m_dir / std::filesystem::path("status");
}

void
BoredomScheduler::set_config_file(const std::filesystem::path& config)
{
    m_configfile = config;
    init();
}

void
BoredomScheduler::init()
{
    if (!std::filesystem::exists(m_dir)) {
        std::filesystem::create_directories(m_dir);
    }

    if (!std::filesystem::exists(m_statusfile)) {
        m_status = BSchedulerStatus::ENABLED;
        std::ofstream statusfile(m_statusfile, std::ofstream::binary);

        statusfile.write(reinterpret_cast<const char*>(&m_status),
                         sizeof(m_status));
        statusfile.close();
    }

    std::ifstream statusfile(m_statusfile, std::ifstream::binary);
    statusfile.read(reinterpret_cast<char*>(&m_status), sizeof(m_status));
    statusfile.close();

    m_config = simpleini::SimpleINI(m_configfile);
}

bool
BoredomScheduler::is_alarm() const
{
    if (m_is_snooze() || m_status == BSchedulerStatus::DISABLED) {
        return false;
    }

    const auto now = std::chrono::system_clock::now();
    const auto now_t = std::chrono::system_clock::to_time_t(now);
    auto now_tm = std::localtime(&now_t);

    const auto now_hms = std::chrono::hh_mm_ss<std::chrono::seconds>{ (
      std::chrono::hours(now_tm->tm_hour) +
      std::chrono::minutes(now_tm->tm_min)) };

    auto bored = parse_from_iniconf(m_config, now);
    return has_unconnected(bored, now_hms);
}

void
BoredomScheduler::snooze(std::chrono::seconds seconds)
{
    m_snooze_t = seconds;
    m_snooze_start = std::chrono::system_clock::now();
}

void
BoredomScheduler::disable()
{
    m_status = BSchedulerStatus::DISABLED;
    std::ofstream statusfile(m_statusfile, std::ofstream::binary);

    statusfile.write(reinterpret_cast<const char*>(&m_status),
                     sizeof(m_status));
    statusfile.close();
}

void
BoredomScheduler::enable()
{
    m_status = BSchedulerStatus::ENABLED;
    std::ofstream statusfile(m_statusfile, std::ofstream::binary);

    statusfile.write(reinterpret_cast<const char*>(&m_status),
                     sizeof(m_status));
    statusfile.close();
}

bool
BoredomScheduler::m_is_snooze() const
{
    return std::chrono::system_clock::now() < (m_snooze_start + m_snooze_t);
}
