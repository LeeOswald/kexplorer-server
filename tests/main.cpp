#include "common.hpp"

#include <kesrv/knownprops.hxx>


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

    Kes::initialize();

    auto ret = RUN_ALL_TESTS();

    Kes::finalize();

    return ret;
}

