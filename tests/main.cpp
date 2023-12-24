#include "common.hpp"

#include <export/knownprops.hxx>


#if KES_DEBUG && defined(_MSC_VER)
#include <crtdbg.h>
#endif

int main(int argc, char** argv)
{
#if KES_DEBUG && defined(_MSC_VER)
    int tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    tmpFlag |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(tmpFlag);
#endif

    ::testing::InitGoogleTest(&argc, argv);

    Kes::ExceptionProps::registerAll();

    return RUN_ALL_TESTS();
}

