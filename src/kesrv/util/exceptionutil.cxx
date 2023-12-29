#include <kesrv/knownprops.hxx>
#include <kesrv/util/exceptionutil.hxx>
#include <kesrv/util/format.hxx>

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

    out << "\n" << indent << " File: " << e.source().source.file_name();
    out << "\n" << indent << " Line: " << e.source().source.line();

    if (!e.source().stack.empty())
    {
        out << "\n" << indent << " Stack:\n";
        out << e.source().stack;
    }

    auto properties = e.properties();
    if (properties)
    {
        for (auto& prop: *properties)
        {
            out << "\n " << indent;

            auto pi = lookupProperty(prop.id);
            if (pi)
            {
                out << pi->name() << ": ";
                pi->format(prop, out);
            }
            else
            {
                out << Util::format(" 0x%08x: ???", prop.id);
            }
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


KESRV_EXPORT std::string formatException(const std::exception& e) noexcept
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

KESRV_EXPORT std::string formatException(const Kes::Exception& e) noexcept
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

KESRV_EXPORT void logException(Log::ILog* log, Log::Level level, const std::exception& e) noexcept
{
    if (level >= log->level())
    {
        auto s = formatException(e);
        log->write(level, "\n%s", s.c_str());
    }
}

KESRV_EXPORT void logException(Log::ILog* log, Log::Level level, const Kes::Exception& e) noexcept
{
    if (level >= log->level())
    {
        auto s = formatException(e);
        log->write(level, "\n%s", s.c_str());
    }
}

} // namespace Util {}

} // namespace Kes {}
