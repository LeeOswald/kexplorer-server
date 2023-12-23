#include <export/knownprops.hxx>
#include <export/util/exceptionutil.hxx>

#include <sstream>

namespace Kes
{

namespace Util
{

namespace
{

void formatException(const Kes::Exception& e, std::ostringstream& out, int level);


void formatException(const std::exception& e, std::ostringstream& out, int level)
{
    if (level > 0)
        out << '\n' << std::string(level * 3, ' ');

    out << e.what();

    try
    {
        std::rethrow_if_nested(e);
    }
    catch (Kes::Exception& nested)
    {
        formatException(nested, out, level + 1);
    }
    catch (std::exception& nested)
    {
        formatException(nested, out, level + 1);
    }
    catch (...)
    {
    }
}

void formatException(const Kes::Exception& e, std::ostringstream& out, int level)
{
    std::string indent(level * 3, ' ');

    if (level > 0)
        out << '\n' << indent;

    auto message = e.message();
    if (message)
    {
        out << *message;
    }
    else
    {
        out << e.what();
    }

    out << "\nFile: " << e.source().file_name();
    out << "\nLine: " << e.source().line();

    auto properties = e.properties();
    if (properties)
    {
        for (auto& prop: *properties)
        {
            out << "\n" << indent << prop.name << ": ";

            if (!Kes::ExceptionProps::format(prop, out))
                out << "???";
        }
    }

    try
    {
        std::rethrow_if_nested(e);
    }
    catch (Kes::Exception& nested)
    {
        formatException(nested, out, level + 1);
    }
    catch (std::exception& nested)
    {
        formatException(nested, out, level + 1);
    }
    catch (...)
    {
    }
}

} // namespace {}


std::string KESCOMMON_EXPORT formatException(const std::exception& e) noexcept
{
    try
    {
        std::ostringstream out;

        formatException(e, out, 0);

        return out.str();
    }
    catch (...)
    {
    }

    return std::string();
}

std::string KESCOMMON_EXPORT formatException(const Kes::Exception& e) noexcept
{
    try
    {
        std::ostringstream out;

        formatException(e, out, 0);

        return out.str();
    }
    catch (...)
    {

    }

    return std::string();
}

void KESCOMMON_EXPORT logException(Log::ILog* log, Log::Level level, const std::exception& e) noexcept
{
    if (level >= log->level())
    {
        auto s = formatException(e);
        log->write(level, "\n%s", s.c_str());
    }
}

void KESCOMMON_EXPORT logException(Log::ILog* log, Log::Level level, const Kes::Exception& e) noexcept
{
    if (level >= log->level())
    {
        auto s = formatException(e);
        log->write(level, "\n%s", s.c_str());
    }
}

} // namespace Util {}

} // namespace Kes {}
