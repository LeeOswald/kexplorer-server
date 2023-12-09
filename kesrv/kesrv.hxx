#pragma once

#include <export/json.hxx>

namespace Kes
{


struct IRequestHandler
{
    virtual bool process(const char* key, const Json::Document& request, Json::Document& response) = 0;

protected:
    virtual ~IRequestHandler() {}
};


struct IRequestProcessor
{
    virtual std::string process(const char* request, size_t length) = 0;
    virtual void registerHandler(const char* key, IRequestHandler* handler) = 0;
    virtual void unregisterHandler(const char* key, IRequestHandler* handler) = 0;

protected:
    virtual ~IRequestProcessor() {}
};


} // namespace Kes {}