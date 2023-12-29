#pragma once

#include <kesrv/platform.hxx>


#if defined(_WIN32) || defined(__CYGWIN__)
    #ifdef KESRV_EXPORTS
        #define KESRV_EXPORT __declspec(dllexport)
    #else
        #define KESRV_EXPORT __declspec(dllimport)
    #endif
#else
    #define KESRV_EXPORT __attribute__((visibility("default")))
#endif


namespace Kes
{

KESRV_EXPORT void initialize();
KESRV_EXPORT void finalize();


enum class CallbackResult
{
    Continue,
    Abort
};

} // namespace Kes {}
