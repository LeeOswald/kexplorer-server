#include "common.hpp"

#include <kesrv/fixedstring.hxx>


TEST(Kes_FixedString, construct)
{
    Kes::FixedString<10> s;
    EXPECT_EQ(s.length(), 0);
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(*s.c_str(), '\0');

    Kes::FixedString<10> s0("");
    EXPECT_EQ(s0.length(), 0);
    EXPECT_TRUE(s0.empty());
    EXPECT_EQ(*s0.c_str(), '\0');

    Kes::FixedString<10> s1("abcdef");
    EXPECT_EQ(s1.length(), 6);
    EXPECT_FALSE(s1.empty());
    EXPECT_EQ(std::strcmp(s1.c_str(), "abcdef"), 0);

    Kes::FixedString<3> s2("abcdef");
    EXPECT_EQ(s2.length(), 3);
    EXPECT_FALSE(s2.empty());
    EXPECT_EQ(std::strcmp(s2.c_str(), "abc"), 0);

    Kes::FixedString<20> s3(s1);
    EXPECT_EQ(s3.length(), 6);
    EXPECT_FALSE(s3.empty());
    EXPECT_EQ(std::strcmp(s3.c_str(), "abcdef"), 0);

    Kes::FixedString<3> s4(s1);
    EXPECT_EQ(s4.length(), 3);
    EXPECT_FALSE(s4.empty());
    EXPECT_EQ(std::strcmp(s4.c_str(), "abc"), 0);
}

TEST(Kes_FixedString, iterate)
{
    {
        Kes::FixedString<10> s;
        EXPECT_EQ(s.begin(), s.end());
        EXPECT_EQ(s.rbegin(), s.rend());
    }

    {
        Kes::FixedString<10> s("abcdef");
        auto it = s.begin();
        EXPECT_NE(it, s.end());
        EXPECT_EQ(*it, 'a');
        ++it;
        EXPECT_NE(it, s.end());
        EXPECT_EQ(*it, 'b');
        ++it;
        EXPECT_NE(it, s.end());
        EXPECT_EQ(*it, 'c');
        ++it;
        EXPECT_NE(it, s.end());
        EXPECT_EQ(*it, 'd');
        ++it;
        EXPECT_NE(it, s.end());
        EXPECT_EQ(*it, 'e');
        ++it;
        EXPECT_NE(it, s.end());
        EXPECT_EQ(*it, 'f');
        ++it;
        EXPECT_EQ(it, s.end());
    }

    {
        Kes::FixedString<10> s("abcdef");

        auto it = s.rbegin();
        EXPECT_NE(it, s.rend());
        EXPECT_EQ(*it, 'f');
        ++it;
        EXPECT_NE(it, s.rend());
        EXPECT_EQ(*it, 'e');
        ++it;
        EXPECT_NE(it, s.rend());
        EXPECT_EQ(*it, 'd');
        ++it;
        EXPECT_NE(it, s.rend());
        EXPECT_EQ(*it, 'c');
        ++it;
        EXPECT_NE(it, s.rend());
        EXPECT_EQ(*it, 'b');
        ++it;
        EXPECT_NE(it, s.rend());
        EXPECT_EQ(*it, 'a');
        ++it;
        EXPECT_EQ(it, s.rend());
    }
}

TEST(Kes_FixedString, compare)
{
    EXPECT_TRUE(Kes::FixedString<10>().compare(Kes::FixedString<10>()) == 0);
    EXPECT_TRUE(Kes::FixedString<10>("").compare(Kes::FixedString<10>("")) == 0);
    EXPECT_TRUE(Kes::FixedString<10>("abc").compare(Kes::FixedString<10>("abc")) == 0);
    EXPECT_TRUE(Kes::FixedString<10>("abc").compare(Kes::FixedString<20>("abc")) == 0);
    EXPECT_TRUE(Kes::FixedString<10>("abc").compare(Kes::FixedString<10>("def")) < 0);
    EXPECT_TRUE(Kes::FixedString<10>("abc").compare(Kes::FixedString<10>("abcdef")) < 0);
    EXPECT_TRUE(Kes::FixedString<10>("def").compare(Kes::FixedString<10>("abc")) > 0);
    EXPECT_TRUE(Kes::FixedString<10>("abcdef").compare(Kes::FixedString<10>("abc")) > 0);

    EXPECT_TRUE(Kes::FixedString<10>().compare("") == 0);
    EXPECT_TRUE(Kes::FixedString<10>("abc").compare("abc") == 0);
    EXPECT_TRUE(Kes::FixedString<10>("abc").compare("def") < 0);
    EXPECT_TRUE(Kes::FixedString<10>("abc").compare("abcdef") < 0);
    EXPECT_TRUE(Kes::FixedString<10>("def").compare("abc") > 0);
    EXPECT_TRUE(Kes::FixedString<10>("abcdef").compare("abc") > 0);
}

TEST(Kes_FixedString, 3w_compare)
{
    EXPECT_TRUE(Kes::FixedString<10>() == Kes::FixedString<10>());
    EXPECT_TRUE(Kes::FixedString<10>("") == Kes::FixedString<10>(""));
    EXPECT_TRUE(Kes::FixedString<10>("abc") == Kes::FixedString<10>("abc"));
    EXPECT_TRUE(Kes::FixedString<10>("abc") == Kes::FixedString<20>("abc"));

    EXPECT_TRUE(Kes::FixedString<10>("abc") != Kes::FixedString<10>("def"));
    EXPECT_TRUE(Kes::FixedString<10>("abc") != Kes::FixedString<10>("abcdef"));
    EXPECT_TRUE(Kes::FixedString<10>("def") != Kes::FixedString<10>("abc"));
    EXPECT_TRUE(Kes::FixedString<10>("abcdef") != Kes::FixedString<10>("abc"));

    EXPECT_TRUE(Kes::FixedString<10>("abc") < Kes::FixedString<10>("def"));
    EXPECT_TRUE(Kes::FixedString<10>("abc") < Kes::FixedString<10>("abcdef"));

    EXPECT_TRUE(Kes::FixedString<10>("def") > Kes::FixedString<10>("abc"));
    EXPECT_TRUE(Kes::FixedString<10>("abcdef") > Kes::FixedString<10>("abc"));

    EXPECT_TRUE(Kes::FixedString<10>() == "");
    EXPECT_TRUE(Kes::FixedString<10>("abc") == "abc");

    EXPECT_TRUE(Kes::FixedString<10>("abc") != "def");
    EXPECT_TRUE(Kes::FixedString<10>("abc") != "abcdef");
    EXPECT_TRUE(Kes::FixedString<10>("def") != "abc");
    EXPECT_TRUE(Kes::FixedString<10>("abcdef") != "abc");

    EXPECT_TRUE(Kes::FixedString<10>("abc") < "def");
    EXPECT_TRUE(Kes::FixedString<10>("abc") < "abcdef");

    EXPECT_TRUE(Kes::FixedString<10>("def") > "abc");
    EXPECT_TRUE(Kes::FixedString<10>("abcdef") > "abc");
}
