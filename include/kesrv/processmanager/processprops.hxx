#pragma once

#include <kesrv/propertybag.hxx>

namespace Kes
{

namespace ProcessProps
{

namespace Private
{

void registerAll();

} // namespace Private {}


using Process = PropertyInfo<PropertyBag::Table, KES_PROPID("process.process"), "Process Info", NullPropertyFormatter>;
using DeletedProcess = PropertyInfo<int, KES_PROPID("process.deleted_process"), "Deleted Process", NullPropertyFormatter>;
using ProcessList = PropertyInfo<PropertyBag::Array, KES_PROPID("process.process_list"), "Process List", NullPropertyFormatter, Process>;
using DeletedProcessList = PropertyInfo<PropertyBag::Array, KES_PROPID("process.deleted_process_list"), "Deleted Process List", NullPropertyFormatter, int>;

using Error = PropertyInfo<std::string, KES_PROPID("process.error"), "__Error", PropertyFormatter<std::string>>;
using Newcomer = PropertyInfo<bool, KES_PROPID("process.newcomer"), "__New", PropertyFormatter<bool>>;
using Pid = PropertyInfo<int, KES_PROPID("process.pid"), "PID", PropertyFormatter<int>>;
using PPid = PropertyInfo<int, KES_PROPID("process.ppid"), "Parent PID", PropertyFormatter<int>>;
using PGrp = PropertyInfo<int, KES_PROPID("process.pgrp"), "Process Group ID", PropertyFormatter<int>>;
using Tpgid = PropertyInfo<int, KES_PROPID("process.tpgid"), "Process Group ID of the Terminal", PropertyFormatter<int>>;
using Session = PropertyInfo<int, KES_PROPID("process.session"), "Session ID", PropertyFormatter<int>>;
using Ruid = PropertyInfo<int, KES_PROPID("process.ruid"), "User ID", PropertyFormatter<int>>;
using StatComm = PropertyInfo<std::string, KES_PROPID("process.stat_comm"), "Command Name", PropertyFormatter<std::string>>;
using Comm = PropertyInfo<std::string, KES_PROPID("process.comm"), "Program Name", PropertyFormatter<std::string>>;
using CmdLine = PropertyInfo<std::string, KES_PROPID("process.cmdline"), "Command Line", PropertyFormatter<std::string>>;
using Exe = PropertyInfo<std::string, KES_PROPID("process.exe"), "Executable Name", PropertyFormatter<std::string>>;

} // namespace ProcessProps {}

} // namespace Kes {}