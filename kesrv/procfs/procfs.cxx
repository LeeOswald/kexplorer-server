#include "procfs.hxx"

#include <fstream>


namespace Kes
{

namespace ProcFs
{

std::string getProcFsPath()
{
    return std::string("/proc");
}

std::optional<Stat> readStat(pid_t pid)
{
    auto path = getProcFsPath();
    path.append("/");
    path.append(std::to_string(pid));
    path.append("/stat");

    std::ifstream stat(path);
    if (!stat.good())
    {
        //LogWarning(getLog(), "Failed to open %s", path.c_str());
        return std::nullopt;
    }

    Stat result;

    std::string s;
    std::getline(stat, s);

    try
    {
        auto start = s.c_str();
        auto pEnd = start + s.length();
        auto end = start;
        size_t index = 0;
        while (start < pEnd)
        {
            // look for the field end
            while ((end < pEnd) && *end && !std::isspace(*end))
            {
                if (*end == '(')
                {
                    end = std::strrchr(end, ')'); // avoid process names like ":-) 1 2 3"
                    if (!end || !*end)
                    {
                        //LogError(getLog(), "Invalid stat record in %s", path.c_str());
                        return std::nullopt;
                    }
                }

                ++end;
            }

            if (end > start)
            {
                char* tmp = nullptr;
                switch (index)
                {
                case 0:
                    result.pid = std::strtol(start, &tmp, 10);
                    break;
                case 1:
                    if (end > start + 3)
                        result.comm.assign(start + 2, end - 1);
                    break;
                case 2:
                    result.state = *(start + 1);
                    break;
                case 3:
                    result.ppid = std::strtol(start + 1, &tmp, 10);
                    break;
                case 4:
                    result.pgrp = std::strtod(start + 1, &tmp);
                    break;
                case 5:
                    result.session = std::strtod(start + 1, &tmp);
                    break;
                case 6:
                    result.tty_nr = std::strtod(start + 1, &tmp);
                    break;
                case 7:
                    result.tpgid = std::strtod(start + 1, &tmp);
                    break;
                case 8:
                    result.flags = (unsigned)std::strtoul(start + 1, &tmp, 10);
                    break;
                case 9:
                    result.minflt = std::strtoul(start + 1, &tmp, 10);
                    break;
                case 10:
                    result.cminflt = std::strtoul(start + 1, &tmp, 10);
                    break;
                case 11:
                    result.majflt = std::strtoul(start + 1, &tmp, 10);
                    break;
                case 12:
                    result.cmajflt = std::strtoul(start + 1, &tmp, 10);
                    break;
                case 13:
                    result.utime = std::strtoul(start + 1, &tmp, 10);
                    break;
                case 14:
                    result.stime = std::strtoul(start + 1, &tmp, 10);
                    break;
                case 15:
                    result.cutime = std::strtol(start + 1, &tmp, 10);
                    break;
                case 16:
                    result.cstime = std::strtol(start + 1, &tmp, 10);
                    break;
                case 17:
                    result.priority = std::strtol(start + 1, &tmp, 10);
                    break;
                case 18:
                    result.nice = std::strtol(start + 1, &tmp, 10);
                    break;
                case 19:
                    result.num_threads = std::strtol(start + 1, &tmp, 10);
                    break;
                case 20:
                    result.itrealvalue = std::strtol(start + 1, &tmp, 10);
                    break;
                case 21:
                    result.starttime = std::strtoull(start + 1, &tmp, 10);
                    break;
                case 22:
                    result.vsize = std::strtoul(start + 1, &tmp, 10);
                    break;
                case 23:
                    result.rss = std::strtol(start + 1, &tmp, 10);
                    break;
                case 24:
                    result.rsslim = std::strtoul(start + 1, &tmp, 10);
                    break;
                case 25:
                    result.startcode = std::strtoul(start + 1, &tmp, 10);
                    break;
                case 26:
                    result.endcode = std::strtoul(start + 1, &tmp, 10);
                    break;
                case 27:
                    result.startstack = std::strtoul(start + 1, &tmp, 10);
                    break;
                case 28:
                    result.kstkesp = std::strtoul(start + 1, &tmp, 10);
                    break;
                case 29:
                    result.kstkeip = std::strtoul(start + 1, &tmp, 10);
                    break;
                case 30:
                    result.signal = std::strtoul(start + 1, &tmp, 10);
                    break;
                case 31:
                    result.blocked = std::strtoul(start + 1, &tmp, 10);
                    break;
                case 32:
                    result.sigignore = std::strtoul(start + 1, &tmp, 10);
                    break;
                case 33:
                    result.sigcatch = std::strtoul(start + 1, &tmp, 10);
                    break;
                case 34:
                    result.wchan = std::strtoul(start + 1, &tmp, 10);
                    break;
                case 35:
                    result.nswap = std::strtoul(start + 1, &tmp, 10);
                    break;
                case 36:
                    result.cnswap = std::strtoul(start + 1, &tmp, 10);
                    break;
                case 37:
                    result.exit_signal = std::strtod(start + 1, &tmp);
                    break;
                case 38:
                    result.processor = std::strtod(start + 1, &tmp);
                    break;
                case 39:
                    result.rt_priority = (unsigned)std::strtoul(start + 1, &tmp, 10);
                    break;
                case 40:
                    result.policy = (unsigned)std::strtoul(start + 1, &tmp, 10);
                    break;
                case 41:
                    result.delayacct_blkio_ticks = std::strtoull(start + 1, &tmp, 10);
                    break;
                case 42:
                    result.guest_time = std::strtoul(start + 1, &tmp, 10);
                    break;
                case 43:
                    result.cguest_time = std::strtol(start + 1, &tmp, 10);
                    break;
                case 44:
                    result.start_data = std::strtoul(start + 1, &tmp, 10);
                    break;
                case 45:
                    result.end_data = std::strtoul(start + 1, &tmp, 10);
                    break;
                case 46:
                    result.start_brk = std::strtoul(start + 1, &tmp, 10);
                    break;
                case 47:
                    result.arg_start = std::strtoul(start + 1, &tmp, 10);
                    break;
                case 48:
                    result.arg_end = std::strtoul(start + 1, &tmp, 10);
                    break;
                case 49:
                    result.env_start = std::strtoul(start + 1, &tmp, 10);
                    break;
                case 50:
                    result.env_end = std::strtoul(start + 1, &tmp, 10);
                    break;
                case 51:
                    result.exit_code = std::strtod(start + 1, &tmp);
                    break;
                }
            }

            ++index;
            start = end;
            ++end;
        }
    }
    catch (std::exception& e)
    {
        //LogWarning(getLog(), "Failed to parse %s: %s", path.c_str(), e.what());
        return std::nullopt;
    }

    return result;
}

} // namespace ProcFs {}

} // namespace Kes {}
