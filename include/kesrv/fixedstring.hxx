#pragma once

#include <kesrv/kesrv.hxx>

#include <compare>
#include <iterator>
#include <ostream>


namespace Kes
{

//
// immutable fixed-size null-terminated string
//

template <std::size_t N, typename CharT = char, typename CharTraits = std::char_traits<CharT>>
class FixedString final
{
    static_assert(!std::is_array_v<CharT>);
    static_assert(std::is_trivial_v<CharT> && std::is_standard_layout_v<CharT>);
    static_assert(std::is_same_v<CharT, typename CharTraits::char_type>);

public:
    static constexpr std::size_t Size = N;

    using traits_type = CharTraits;
    using value_type = CharT;
    using pointer = value_type*;
    using const_pointer = value_type const*;
    using reference = value_type&;
    using const_reference = value_type const&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using const_iterator = const_pointer;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    constexpr FixedString() noexcept
    {
        m_str[0] = CharT();
    }

    constexpr FixedString(const std::basic_string_view<CharT, CharTraits>& src) noexcept
    {
        const size_type count = std::min(src.length(), N);
        traits_type::copy(m_str, src.data(), count);
        m_str[count] = CharT();
        m_length = count;
    }

    template <size_t _N>
    constexpr FixedString(const FixedString<_N, value_type, traits_type>& o)
        : FixedString(std::basic_string_view<CharT, CharTraits>(o.c_str(), o.length()))
    {
    }

    FixedString& operator=(const FixedString&) = delete;

    constexpr size_type length() const noexcept
    {
        return m_length;
    }

    [[nodiscard]] constexpr bool empty() const noexcept
    {
        return m_length == 0;
    }

    constexpr const_pointer c_str() const noexcept
    {
        return m_str;
    }

    constexpr const_pointer data() const noexcept
    {
        return m_str;
    }

    constexpr const_iterator begin() const noexcept
    {
        return m_str;
    }

    constexpr const_iterator end() const noexcept
    {
        return m_str + m_length;
    }

    constexpr const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    constexpr const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    constexpr const_reference operator[](size_type i) const noexcept
    {
        assert(i <= N);
        return m_str[i];
    }

    constexpr int compare(const std::basic_string_view<CharT, CharTraits>& o) const noexcept
    {
        const size_type l = std::min(m_length, o.length());
        int result = traits_type::compare(m_str, o.data(), l);
        if (result == 0)
            result = compare(m_length, o.length());
        return result;
    }

    template <std::size_t _N>
    constexpr int compare(const FixedString<_N, CharT, CharTraits>& o) const noexcept
    {
        return compare(std::basic_string_view<CharT, CharTraits>(o.data(), o.length()));
    }

    constexpr std::strong_ordering operator<=>(const FixedString& o) const noexcept
    {
        auto v = compare(o);
        if (v == 0)
            return std::strong_ordering::equal;
        else if (v < 0)
            return std::strong_ordering::less;
        return std::strong_ordering::greater;
    }

    constexpr std::strong_ordering operator<=>(const std::basic_string_view<CharT, CharTraits>& o) const noexcept
    {
        auto v = compare(o);
        if (v == 0)
            return std::strong_ordering::equal;
        else if (v < 0)
            return std::strong_ordering::less;
        return std::strong_ordering::greater;
    }

private:
    static constexpr int compare(size_type n1, size_type n2) noexcept
    {
        const difference_type diff = n1 - n2;
        if (diff > std::numeric_limits<int>::max())
            return std::numeric_limits<int>::max();
        if (diff < std::numeric_limits<int>::min())
            return std::numeric_limits<int>::min();
        return static_cast<int>(diff);
    }

    CharT m_str[N + 1];
    size_type m_length = 0;
};


template <std::size_t N1, std::size_t N2, typename CharT, typename CharTraits>
inline constexpr bool operator==(const FixedString<N1, CharT, CharTraits>& a, const FixedString<N2, CharT, CharTraits>& b) noexcept
{
    return (a.compare(b) == 0);
}

template <std::size_t N1, std::size_t N2, typename CharT, typename CharTraits>
inline constexpr bool operator==(const FixedString<N1, CharT, CharTraits>& a, const std::basic_string_view<CharT, CharTraits>& b) noexcept
{
    return (a.compare(b) == 0);
}

template <std::size_t N, typename CharT, typename CharTraits>
inline constexpr bool operator==(const FixedString<N, CharT, CharTraits>& a, const char* b) noexcept
{
    return (a.compare(b) == 0);
}


template <std::size_t N, typename CharT, typename CharTraits>
inline std::basic_ostream<CharT, CharTraits>& operator<<(std::basic_ostream<CharT, CharTraits>& os, const FixedString<N, CharT, CharTraits>& str)
{
    os << std::basic_string_view<CharT, CharTraits>(str.data(), str.length());
    return os;
}


} // namespace Kes {}

