#include "common.hpp"

#include <kesrv/knownprops.hxx>
#include <kesrv/propertybag.hxx>

#include <mutex>

static std::once_flag g_propsRegistered;

using BoolProperty = Kes::PropertyInfo<bool, KES_PROPID("BoolProperty"), "bool", Kes::PropertyFormatter<bool>>;
using IntProperty = Kes::PropertyInfo<int, KES_PROPID("IntProperty"), "int", Kes::PropertyFormatter<int>>;
using UIntProperty = Kes::PropertyInfo<unsigned int, KES_PROPID("UIntProperty"), "unsigned int", Kes::PropertyFormatter<unsigned int>>;
using Int64Property = Kes::PropertyInfo<int64_t, KES_PROPID("Int64Property"), "int64_t", Kes::PropertyFormatter<int64_t>>;
using UInt64Property = Kes::PropertyInfo<uint64_t, KES_PROPID("UInt64Property"), "uint64_t", Kes::PropertyFormatter<uint64_t>>;
using StringProperty = Kes::PropertyInfo<std::string, KES_PROPID("StringProperty"), "std::string", Kes::PropertyFormatter<std::string>>;

void registerProps()
{
    std::call_once(
        g_propsRegistered,
        []()
        {
            Kes::registerProperty(new Kes::PropertyInfoWrapper<BoolProperty>);
            Kes::registerProperty(new Kes::PropertyInfoWrapper<IntProperty>);
            Kes::registerProperty(new Kes::PropertyInfoWrapper<UIntProperty>);
            Kes::registerProperty(new Kes::PropertyInfoWrapper<Int64Property>);
            Kes::registerProperty(new Kes::PropertyInfoWrapper<UInt64Property>);
            Kes::registerProperty(new Kes::PropertyInfoWrapper<StringProperty>);
        } 
    );
}


TEST(Kes_PropertyBag, propertyFromJson)
{
    registerProps();

    {
        char json[] = "{\"BoolProperty\":true,\"IntProperty\":-2,\"UIntProperty\":3,\"Int64Property\":-19,\"UInt64Property\":55,\"StringProperty\":\"this is a string\"}";
        Kes::Json::Document doc;
        doc.ParseInsitu(json);
        ASSERT_FALSE(doc.HasParseError());
        auto m = doc.MemberBegin();
        ASSERT_NE(m, doc.MemberEnd());

        {
            auto prop = Kes::propertyFromJson(m->name.GetString(), m->value, Logger::instance());
            EXPECT_EQ(prop.id, BoolProperty::id());
            ASSERT_NE(prop.info, nullptr);
            EXPECT_STREQ(prop.info->idstr(), BoolProperty::idstr());
            EXPECT_EQ(std::any_cast<bool>(prop.value), true);
        }

        ++m;
        ASSERT_NE(m, doc.MemberEnd());

        {
            auto prop = Kes::propertyFromJson(m->name.GetString(), m->value, Logger::instance());
            EXPECT_EQ(prop.id, IntProperty::id());
            ASSERT_NE(prop.info, nullptr);
            EXPECT_STREQ(prop.info->idstr(), IntProperty::idstr());
            EXPECT_EQ(std::any_cast<int>(prop.value), -2);
        }

        ++m;
        ASSERT_NE(m, doc.MemberEnd());

        {
            auto prop = Kes::propertyFromJson(m->name.GetString(), m->value, Logger::instance());
            EXPECT_EQ(prop.id, UIntProperty::id());
            ASSERT_NE(prop.info, nullptr);
            EXPECT_STREQ(prop.info->idstr(), UIntProperty::idstr());
            EXPECT_EQ(std::any_cast<unsigned int>(prop.value), 3);
        }

        ++m;
        ASSERT_NE(m, doc.MemberEnd());

        {
            auto prop = Kes::propertyFromJson(m->name.GetString(), m->value, Logger::instance());
            EXPECT_EQ(prop.id, Int64Property::id());
            ASSERT_NE(prop.info, nullptr);
            EXPECT_STREQ(prop.info->idstr(), Int64Property::idstr());
            EXPECT_EQ(std::any_cast<int64_t>(prop.value), -19);
        }

        ++m;
        ASSERT_NE(m, doc.MemberEnd());

        {
            auto prop = Kes::propertyFromJson(m->name.GetString(), m->value, Logger::instance());
            EXPECT_EQ(prop.id, UInt64Property::id());
            ASSERT_NE(prop.info, nullptr);
            EXPECT_STREQ(prop.info->idstr(), UInt64Property::idstr());
            EXPECT_EQ(std::any_cast<uint64_t>(prop.value), 55);
        }

        ++m;
        ASSERT_NE(m, doc.MemberEnd());

        {
            auto prop = Kes::propertyFromJson(m->name.GetString(), m->value, Logger::instance());
            EXPECT_EQ(prop.id, StringProperty::id());
            ASSERT_NE(prop.info, nullptr);
            EXPECT_STREQ(prop.info->idstr(), StringProperty::idstr());
            EXPECT_STREQ(std::any_cast<std::string>(prop.value).c_str(), "this is a string");
        }

        {
            auto prop = Kes::propertyFromJson("xxx", m->value, Logger::instance());
            EXPECT_FALSE(prop.value.has_value());
            EXPECT_EQ(prop.id, Kes::InvalidPropId);
        }
        
        ++m;
        ASSERT_EQ(m, doc.MemberEnd());
    }
}