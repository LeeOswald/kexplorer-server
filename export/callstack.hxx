#pragma once


#include <export/platform.hxx>

#include <vector>


namespace Kes
{

using StackFrames = std::vector<uint64_t>;

struct DecodedStackFrame
{
    uint64_t address;
    std::string file;
    std::string function;
    std::string decoratedFunction;

    DecodedStackFrame() = default;

    template <typename FileT, typename FunctionT, typename DecoratedFunctionT>
    explicit DecodedStackFrame(uint64_t address, FileT&& file, FunctionT&& function, DecoratedFunctionT&& decoratedFunction)
        : address(address)
        , file(std::forward<FileT>(file))
        , function(std::forward<FunctionT>(function))
        , decoratedFunction(std::forward<DecoratedFunctionT>(decoratedFunction))
    {}
};


using DecodedStackFrames = std::vector<DecodedStackFrame>;


KESCOMMON_EXPORT StackFrames captureStackTrace(uint32_t framesToSkip);

KESCOMMON_EXPORT DecodedStackFrames decodeStackTrace(const StackFrames& trace);

} // namespace Kes {}
