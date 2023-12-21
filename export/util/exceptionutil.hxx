#pragma once

#include <export/exception.hxx>
#include <export/log.hxx>

namespace Kes
{

namespace Util
{


std::string KESCOMMON_EXPORT formatException(const std::exception& e) noexcept;
std::string KESCOMMON_EXPORT formatException(const Kes::Exception& e) noexcept;

void KESCOMMON_EXPORT logException(Log::ILog* log, Log::Level level, const std::exception& e) noexcept;
void KESCOMMON_EXPORT logException(Log::ILog* log, Log::Level level, const Kes::Exception& e) noexcept;


} // namespace Util {}

} // namespace Kes {}

