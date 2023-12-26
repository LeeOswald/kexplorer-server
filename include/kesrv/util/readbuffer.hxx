#pragma once


#include <kesrv/kesrv.hxx>

#include <vector>


namespace Kes
{

namespace Util
{

class ReadBuffer final
{
public:
    using Ptr = std::shared_ptr<ReadBuffer>;

    explicit ReadBuffer(size_t size) noexcept
        : m_buffers { std::vector<char>(size), std::vector<char>(size) }
        , m_size(size)
    {
    }

    ReadBuffer(const ReadBuffer&) = delete;
    ReadBuffer& operator=(const ReadBuffer&) = delete;

    ReadBuffer(ReadBuffer&&) = delete;
    ReadBuffer& operator=(ReadBuffer&&) = delete;

    static Ptr create(size_t size)
    {
        return std::make_shared<ReadBuffer>(size);
    }

    char* data(size_t index) noexcept { return m_buffers[index].data(); }
    const char* data(size_t index) const noexcept { return m_buffers[index].data(); }
    size_t size() const noexcept { return m_size; }
    size_t w_index() const noexcept { return m_active; }
    size_t r_index() const noexcept { return m_active ^ 1; }
    void swap() noexcept { m_active ^= 1; }

private:
    std::vector<char> m_buffers[2];
    size_t m_size;
    size_t m_active = 0;
};


} // namespace Util {}

} // namespace Kes {}
