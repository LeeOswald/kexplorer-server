#pragma once

#include <kesrv/exception.hxx>
#include <kesrv/log.hxx>

namespace Kes
{

namespace Util
{


std::string KESRV_EXPORT formatException(const std::exception& e) noexcept;
std::string KESRV_EXPORT formatException(const Kes::Exception& e) noexcept;

void KESRV_EXPORT logException(Log::ILog* log, Log::Level level, const std::exception& e) noexcept;
void KESRV_EXPORT logException(Log::ILog* log, Log::Level level, const Kes::Exception& e) noexcept;


} // namespace Util {}

} // namespace Kes {}

