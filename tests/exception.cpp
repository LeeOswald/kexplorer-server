#include "common.hpp"

#include <export/exception.hxx>
#include <export/knownprops.hxx>
#include <export/util/exceptionutil.hxx>


TEST(Kes_Exception, simple)
{
    try
    {
        throw Kes::Exception(KES_HERE(), "Test exception");
    }
    catch (Kes::Exception& e)
    {
        EXPECT_STREQ(e.what(), "Test exception");
    }
}

TEST(Kes_Exception, props)
{
    try
    {
        throw Kes::Exception(KES_HERE(), "Test exception 2").add("prop1", 1, "value1").add("prop2", 2, 1991);
    }
    catch (Kes::Exception& e)
    {
        EXPECT_STREQ(e.what(), "Test exception 2");

        auto props = e.properties();
        ASSERT_NE(props, nullptr);
        EXPECT_EQ(props->size(), 2);

        auto p0 = e.find(1);
        ASSERT_NE(p0, nullptr);
        EXPECT_EQ(p0->id, 1);
        EXPECT_STREQ(p0->name.c_str(), "prop1");
        EXPECT_STREQ(std::any_cast<const char*>(p0->value), "value1");

        auto p1 = e.find(2);
        ASSERT_NE(p1, nullptr);
        EXPECT_EQ(p1->id, 2);
        EXPECT_STREQ(p1->name.c_str(), "prop2");
        EXPECT_EQ(std::any_cast<int>(p1->value), 1991);
    }
}

TEST(Kes_Exception, known_props)
{
    try
    {
        throw Kes::Exception(KES_HERE(), "POSIX exception", Kes::ExceptionProps::PosixErrorCode(ENOENT), Kes::ExceptionProps::DecodedError("ENOENT"));
    }
    catch (Kes::Exception& e)
    {
        EXPECT_STREQ(e.what(), "POSIX exception");

        auto props = e.properties();
        ASSERT_NE(props, nullptr);
        EXPECT_EQ(props->size(), 2);

        auto code = e.find(Kes::ExceptionProps::PosixErrorCode::Id::value);
        ASSERT_NE(code, nullptr);
        EXPECT_EQ(code->id, Kes::ExceptionProps::PosixErrorCode::Id::value);
        EXPECT_STREQ(code->name.c_str(), Kes::ExceptionProps::PosixErrorCode::name());
        EXPECT_EQ(std::any_cast<int>(code->value), ENOENT);

        auto text = e.find(Kes::ExceptionProps::DecodedError::Id::value);
        ASSERT_NE(text, nullptr);
        EXPECT_EQ(text->id, Kes::ExceptionProps::DecodedError::Id::value);
        EXPECT_STREQ(text->name.c_str(), Kes::ExceptionProps::DecodedError::name());
        EXPECT_STREQ(std::any_cast<std::string>(text->value).c_str(), "ENOENT");

    }
}

TEST(Kes_Exception, format1)
{
    try
    {
        try
        {
            try
            {
                throw std::bad_alloc();
            }
            catch (std::exception&)
            {
                std::throw_with_nested(std::runtime_error("Level 2 exception"));
            }
        }
        catch (std::exception&)
        {
            std::throw_with_nested(std::runtime_error("Level 3 exception"));
        }
    }
    catch (std::exception& e)
    {
        auto s = Kes::Util::formatException(e);
        std::cout << s << "\n";
    }
}

TEST(Kes_Exception, format2)
{
    try
    {
        try
        {
            try
            {
                throw std::bad_alloc();
            }
            catch (std::exception& e)
            {
                std::throw_with_nested(
                    Kes::Exception(KES_HERE(), "POSIX exception", Kes::ExceptionProps::PosixErrorCode(ENOENT), Kes::ExceptionProps::DecodedError("ENOENT"))
                );
            }
        }
        catch (std::exception& e)
        {
            std::throw_with_nested(std::runtime_error("Level 3 exception"));
        }
    }
    catch (std::exception& e)
    {
        auto s = Kes::Util::formatException(e);
        std::cout << s << "\n";
    }
}
