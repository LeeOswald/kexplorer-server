#pragma once

#include <kesrv/propertybag.hxx>


namespace Kes
{


struct IRequestHandler
{
    virtual bool process(uint32_t sessionId, const char* key, const PropertyBag& request, PropertyBag& response) = 0;
    virtual void startSession(uint32_t id) = 0;
    virtual void endSession(uint32_t id) = 0;

protected:
    virtual ~IRequestHandler() {}
};


struct IRequestProcessor
{
    virtual std::string process(uint32_t sessionId, char* request, size_t length) = 0;
    virtual void registerHandler(const char* key, IRequestHandler* handler) = 0;
    virtual void unregisterHandler(const char* key, IRequestHandler* handler) = 0;
    virtual void startSession(uint32_t id) = 0;
    virtual void endSession(uint32_t id) = 0;

protected:
    virtual ~IRequestProcessor() {}
};


} // namespace Kes {}

