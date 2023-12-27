#include <kesrv/time.hxx>

#include <iomanip>
#include <sstream>

namespace Kes
{

Time::Time(time_t packed) noexcept
{
    struct tm t = {};
    ::gmtime_r(&packed, &t);

    year = t.tm_year;
    month = Month(t.tm_mon);
    day = t.tm_mday;
    hours = t.tm_hour;
    minutes = t.tm_min;
    seconds = t.tm_sec;
    milliseconds = 0;
}

time_t Time::pack() const noexcept
{
    struct tm t = {};
    t.tm_year = year;
    t.tm_mon = month;
    t.tm_mday = day;
    t.tm_hour = hours;
    t.tm_min = minutes;
    t.tm_sec = seconds;

    return ::mktime(&t);
}

KESRV_EXPORT std::string formatTime(const char* format, const Time& time)
{
    struct tm t = {};
    t.tm_year = time.year;
    t.tm_mon = time.month;
    t.tm_mday = time.day;
    t.tm_hour = time.hours;
    t.tm_min = time.minutes;
    t.tm_sec = time.seconds;

    std::basic_ostringstream<char> ss;
    ss << std::put_time(&t, format);

    return ss.str();
}

} // namespace Kes {}