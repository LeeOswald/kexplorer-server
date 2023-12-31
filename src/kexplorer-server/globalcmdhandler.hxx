#pragma once

#include <kesrv/condition.hxx>
#include <kesrv/log.hxx>
#include <kesrv/requestprocessor.hxx>

namespace Kes
{

namespace Private
{

class GlobalCmdHandler final
    : public IRequestHandler
    , public boost::noncopyable
{
public:
    ~GlobalCmdHandler();
    explicit GlobalCmdHandler(IRequestProcessor* rp, Condition& exitCondition, Log::ILog* log);

    bool process(uint32_t sessionId, const char* key, Kes::Request::Id id, const PropertyBag& request, PropertyBag& response) override;
    void startSession(uint32_t id) override;
    void endSession(uint32_t id) override;

private:
    IRequestProcessor* m_rp;
    Condition& m_exitCondition;
    Log::ILog* m_log;
};


} // namespace Private {}

} // namespace Kes {}
