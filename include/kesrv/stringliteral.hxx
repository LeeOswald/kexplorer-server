#pragma once

#include <kesrv/kesrv.hxx>


namespace Kes
{

template<size_t N>
struct StringLiteral
{
    static constexpr size_t size = N;

    constexpr StringLiteral(const char (&str)[N])
    {
        std::copy_n(str, N, value);
    }

    char value[N];
};


template <StringLiteral L>
inline constexpr const char* fromStringLiteral() noexcept
{
    return L.value;
}


} // namespace Kes {}
