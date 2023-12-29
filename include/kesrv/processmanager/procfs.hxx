#pragma once

#include <kesrv/log.hxx>

#include <vector>


namespace Kes
{

namespace ProcFs
{

constexpr pid_t InvalidPid = pid_t(-1);
constexpr pid_t KernelPid = 0;

struct KESRV_EXPORT Stat
{
    bool valid = false;
    std::string error;

    /* 0*/ pid_t pid = InvalidPid;
    /* 1*/ std::string comm;
    /* 2*/ char state = '?';
    /* 3*/ pid_t ppid = InvalidPid;
    /* 4*/ pid_t pgrp = InvalidPid;
    /* 5*/ pid_t session = InvalidPid;
    /* 6*/ int tty_nr = -1;
    /* 7*/ pid_t tpgid = InvalidPid;
    /* 8*/ unsigned flags = 0;
    /* 9*/ unsigned long minflt = 0;                     // minor faults
    /*10*/ unsigned long cminflt = 0;                    // minor faults of waited children
    /*11*/ unsigned long majflt = 0;                     // major faults
    /*12*/ unsigned long cmajflt = 0;                    // major faults of waited children
    /*13*/ unsigned long utime = 0;
    /*14*/ unsigned long stime = 0;
    /*15*/ long cutime = 0;
    /*16*/ long cstime = 0;
    /*17*/ long priority = 0;
    /*18*/ long nice = 0;
    /*19*/ long num_threads = 0;
    /*20*/ long itrealvalue = 0;                         // obsolete
    /*21*/ unsigned long long starttime = 0;             // start time (since reboot)
    /*22*/ unsigned long vsize = 0;                      // virtual memory size
    /*23*/ long rss = 0;                                 // resident size
    /*24*/ unsigned long rsslim = 0;
    /*25*/ unsigned long startcode = 0;
    /*26*/ unsigned long endcode = 0;
    /*27*/ unsigned long startstack = 0;
    /*28*/ unsigned long kstkesp = 0;
    /*29*/ unsigned long kstkeip = 0;
    /*30*/ unsigned long signal = 0;
    /*31*/ unsigned long blocked = 0;
    /*32*/ unsigned long sigignore = 0;
    /*33*/ unsigned long sigcatch = 0;
    /*34*/ unsigned long wchan = 0;
    /*35*/ unsigned long nswap = 0;
    /*36*/ unsigned long cnswap = 0;
    /*37*/ int exit_signal = -1;
    /*38*/ int processor = -1;
    /*39*/ unsigned rt_priority = 0;
    /*40*/ unsigned policy = 0;
    /*41*/ unsigned long long delayacct_blkio_ticks = 0;
    /*42*/ unsigned long guest_time = 0;
    /*43*/ long cguest_time = 0;
    /*44*/ unsigned long start_data = 0;
    /*45*/ unsigned long end_data = 0;
    /*46*/ unsigned long start_brk = 0;
    /*47*/ unsigned long arg_start = 0;
    /*48*/ unsigned long arg_end = 0;
    /*49*/ unsigned long env_start = 0;
    /*50*/ unsigned long env_end = 0;
    /*51*/ int exit_code = 0;

    uint64_t startTime;                                // process start time
    uid_t ruid = uid_t(-1);                              // real user ID of process owner

    Stat() = default;
};


class KESRV_EXPORT ProcFs final
{
public:
    explicit ProcFs(Log::ILog* log);

    static std::string root();

    Stat readStat(pid_t pid) noexcept;
    std::string readComm(pid_t pid) noexcept;
    std::string readExePath(pid_t pid) noexcept;
    std::string readCmdLine(pid_t pid) noexcept;

    std::vector<pid_t> enumeratePids() noexcept;

    uint64_t getBootTime() noexcept;

private:
    uint64_t getBootTimeImpl() noexcept;
    uint64_t fromRelativeTime(uint64_t relative) noexcept;

    Log::ILog* m_log;
};





} // namespace ProcFs {}

} // namespace Kes {}
