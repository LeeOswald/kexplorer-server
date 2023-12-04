#pragma once

#include <export/platform.hxx>

namespace Kesctl
{

inline std::string simpleCommand(const std::string& cmd)
{
    return std::string("{\"command\":\"") + cmd + std::string("\"}");
}

} // namespace Kesctl {}
