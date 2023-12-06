#include "requestprocessor.hxx"

#include <export/exception.hxx>
#include <export/util/request.hxx>


namespace Kes
{

namespace Private
{

std::string RequestProcessor::process(const char* request, size_t length)
{
    try
    {
        LogDebug(m_log, "-> %s", request);

        Kes::Json::Document doc;
        doc.Parse(request, length);

        if (!doc.IsObject())
            return Util::Response::fail("Not a JSON object");

        auto requestKey = doc.FindMember("request");
        if (requestKey != doc.MemberEnd())
        {
            auto key = requestKey->value.GetString();
            if (!key)
                return Util::Response::fail("Invalid request");

        }
    }
    catch (std::exception& e)
    {
        m_log->write(Log::Level::Error, "RequestProcessor: %s", e.what());
        return Util::Response::fail(e.what());
    }

    return Util::Response::fail("Unsupported request");
}

} // namespace Private {}

} // namespace Kes {}
