#include <kesrv/callstack.hxx>
#include <kesrv/util/autoptr.hxx>

#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>

namespace Kes
{

KESRV_EXPORT StackFrames captureStackTrace(uint32_t framesToSkip)
{
    constexpr const int StaticBufferFrameCount = 64;

    void* stackBuffer[StaticBufferFrameCount];
    std::vector<void*> dynamicBuffer;
    auto buffer = stackBuffer;
    auto capturedCount = StaticBufferFrameCount;
    auto bufferCount = StaticBufferFrameCount;

    for (;;)
    {
        capturedCount = ::backtrace(buffer, bufferCount);
        if (capturedCount < bufferCount)
            break;

        bufferCount *= 2;
        dynamicBuffer.resize(bufferCount);
        buffer = dynamicBuffer.data();
    }

    StackFrames frames;
    if (static_cast<unsigned>(capturedCount) > framesToSkip)
    {
        const auto framesCount = capturedCount - framesToSkip;
        frames.reserve(framesCount);
        for (auto i = 0u; i < framesCount; ++i)
            frames.push_back(static_cast<uint64_t>(reinterpret_cast<uintptr_t>(buffer[framesToSkip + i])));
    }

    return frames;
}

KESRV_EXPORT DecodedStackFrames decodeStackTrace(const StackFrames& trace)
{
    DecodedStackFrames decoded;
    decoded.reserve(trace.size());

    for (const auto& frame: trace)
    {
        Dl_info dlinfo;
        if (!::dladdr(reinterpret_cast<const void*>(static_cast<uintptr_t>(frame)), &dlinfo))
        {
            decoded.emplace_back(frame, "???", "???", "???");
        }
        else
        {
            if (!dlinfo.dli_sname)
            {
                decoded.emplace_back(frame, dlinfo.dli_fname ? dlinfo.dli_fname : "???", "???", "???");
            }
            else
            {
                int status = 0;
                Util::CrtAutoPtr<char> demangled(abi::__cxa_demangle(dlinfo.dli_sname, nullptr, 0, &status));
                if (status == 0 && demangled)
                {
                    decoded.emplace_back(frame, dlinfo.dli_fname, demangled, dlinfo.dli_sname);
                }
                else
                {
                    decoded.emplace_back(frame, dlinfo.dli_fname, dlinfo.dli_sname, dlinfo.dli_sname);
                }
            }
        }
    }

    return decoded;
}

} // namespace Kes {}
