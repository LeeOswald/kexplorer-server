#pragma once


#include <kesrv/log.hxx>
#include <kesrv/util/generichandle.hxx>


namespace Kes
{

namespace Private
{


class Logger final
    : public Kes::Log::ILog
    , public boost::noncopyable
{
public:
    ~Logger();
    explicit Logger(Kes::Log::Level level, const char* fileName);

    Kes::Log::Level level() const noexcept override;
    bool writev(Kes::Log::Level level, const char* format, va_list args) noexcept override;
    bool write(Kes::Log::Level level, const char* format, ...) noexcept override;

private:
    struct FileCloser
    {
        void operator()(int fd) noexcept
        {
            ::close(fd);
        }
    };

    using File = Util::GenericHandle<int, int, -1, FileCloser>;

    File m_file;
    Kes::Log::Level m_level = Kes::Log::Level::Info;
};


} // namespace Private {}

} // namespace Kes {}
