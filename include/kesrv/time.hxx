#pragma once

#include <kesrv/kesrv.hxx>

namespace Kes
{

struct KESRV_EXPORT Time final
{
    enum Month
    {
        Unknown = -1,
        January = 0,
        February,
        March,
        April,
        May,
        June,
        July,
        August,
        September,
        October,
        November,
        December
    };

    uint16_t year = 0;
    Month month = Unknown;
    uint16_t day = 0;
    uint16_t hours = 0;
    uint16_t minutes = 0;
    uint16_t seconds = 0;
    uint16_t milliseconds = 0; 

    Time() noexcept = default;
    Time(time_t packed) noexcept;
    Time(uint16_t year, Month month, uint16_t day, uint16_t hours, uint16_t minutes, uint16_t seconds, uint16_t milliseconds) noexcept
        : year(year)
        , month(month)
        , day(day)
        , hours(hours)
        , minutes(minutes)
        , seconds(seconds)
        , milliseconds(milliseconds)
    {
    } 

    time_t pack() const noexcept;
};


KESRV_EXPORT std::string formatTime(const char* format, const Time& time); 

} // namespace Kes {}