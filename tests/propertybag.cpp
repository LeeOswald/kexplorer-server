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
using IntArray = Kes::PropertyInfo<Kes::PropertyBag::Array, KES_PROPID("IntArray"), "int[]", Kes::NullPropertyFormatter, int>;

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
            Kes::registerProperty(new Kes::PropertyInfoWrapper<IntArray>);
        } 
    );
}

struct ErrorHandler :
    public Kes::IPropertyErrorHandler
{
    Kes::CallbackResult handle(Kes::SourceLocation where, const std::string& message) noexcept override
    {
        Logger::instance()->write(Kes::Log::Level::Warning, "%s", message.c_str());
        return Kes::CallbackResult::Continue;
    }
};

static ErrorHandler g_errorHandler;


TEST(Kes_PropertyBag, propertyBagFromJson)
{
    registerProps();

    {
        char json[] = 
        "{" \
            "\"IntArray\":[1,2,3]," \
            "\"Table1\":{\"BoolProperty\":true,\"IntProperty\":13}," \
            "\"Int64Property\":-67," \
            "\"Table2\":{" \
                "\"Table21\":{\"StringProperty\":\"some text\",\"BoolProperty\":false}," \
                "\"IntArray\":[0,9,8,7]," \
                "\"IntProperty\":-5" \
            "}" \
        "}";

        auto b = Kes::propertyBagFromJson(json, &g_errorHandler);
        ASSERT_TRUE(b.isTable());
        auto& root = b.table();
        EXPECT_EQ(root.size(), 4);

        {
            auto it = root.find("IntArray");
            ASSERT_NE(it, root.end());
            ASSERT_TRUE(it->second->isArray());
            auto& intArray = it->second->array();
            EXPECT_EQ(intArray.size(), 3);
            
            {
                ASSERT_TRUE(intArray[0]->isProperty());
                EXPECT_EQ(std::any_cast<int>(intArray[0]->property().value), 1);
                ASSERT_TRUE(intArray[1]->isProperty());
                EXPECT_EQ(std::any_cast<int>(intArray[1]->property().value), 2);
                ASSERT_TRUE(intArray[2]->isProperty());
                EXPECT_EQ(std::any_cast<int>(intArray[2]->property().value), 3);
            }
        }

        {
            auto it = root.find("Table1");
            ASSERT_NE(it, root.end());
            ASSERT_TRUE(it->second->isTable());
            auto& table1 = it->second->table();
            EXPECT_EQ(table1.size(), 2);
            
            {
                auto it2 = table1.find("BoolProperty");
                ASSERT_NE(it2, table1.end());
                ASSERT_TRUE(it2->second->isProperty());
                EXPECT_EQ(std::any_cast<bool>(it2->second->property().value), true);
                it2 = table1.find("IntProperty");
                ASSERT_NE(it2, table1.end());
                ASSERT_TRUE(it2->second->isProperty());
                EXPECT_EQ(std::any_cast<int>(it2->second->property().value), 13);
            }
        }

        {
            auto it = root.find("Int64Property");
            ASSERT_NE(it, root.end());
            ASSERT_TRUE(it->second->isProperty());
            EXPECT_EQ(std::any_cast<int64_t>(it->second->property().value), -67);
        }

        {
            auto it = root.find("Table2");
            ASSERT_NE(it, root.end());
            ASSERT_TRUE(it->second->isTable());
            auto& table2 = it->second->table();
            EXPECT_EQ(table2.size(), 3);
            
            {
                auto it2 = table2.find("Table21");
                ASSERT_NE(it2, table2.end());
                ASSERT_TRUE(it2->second->isTable());
                auto& table21 = it2->second->table();
                EXPECT_EQ(table21.size(), 2);
                
                {
                    auto it3 = table21.find("StringProperty");
                    ASSERT_NE(it3, table21.end());
                    ASSERT_TRUE(it3->second->isProperty());
                    EXPECT_STREQ(std::any_cast<std::string>(it3->second->property().value).c_str(), "some text");

                    it3 = table21.find("BoolProperty");
                    ASSERT_NE(it3, table21.end());
                    ASSERT_TRUE(it3->second->isProperty());
                    EXPECT_EQ(std::any_cast<bool>(it3->second->property().value), false);
                }

                it2 = table2.find("IntArray");
                ASSERT_NE(it2, table2.end());
                ASSERT_TRUE(it2->second->isArray());
                auto& array = it2->second->array();
                EXPECT_EQ(array.size(), 4);

                {
                    ASSERT_TRUE(array[0]->isProperty());
                    EXPECT_EQ(std::any_cast<int>(array[0]->property().value), 0);
                    ASSERT_TRUE(array[1]->isProperty());
                    EXPECT_EQ(std::any_cast<int>(array[1]->property().value), 9);
                    ASSERT_TRUE(array[2]->isProperty());
                    EXPECT_EQ(std::any_cast<int>(array[2]->property().value), 8);
                    ASSERT_TRUE(array[3]->isProperty());
                    EXPECT_EQ(std::any_cast<int>(array[3]->property().value), 7);
                }
                
                it2 = table2.find("IntProperty");
                ASSERT_NE(it2, table2.end());
                ASSERT_TRUE(it2->second->isProperty());
                EXPECT_EQ(std::any_cast<int>(it2->second->property().value), -5);
                
            }
        }
    }
}

TEST(Kes_PropertyBag, propertyBagToJson)
{
    registerProps();

    char json[] =
    "{" \
        "\"IntArray\":[1,2,3]," \
        "\"Table1\":{\"BoolProperty\":true,\"IntProperty\":13}," \
        "\"Int64Property\":-67," \
        "\"Table2\":{" \
            "\"Table21\":{\"StringProperty\":\"some text\",\"BoolProperty\":false}," \
            "\"IntArray\":[0,9,8,7]," \
            "\"IntProperty\":-5" \
        "}" \
    "}";

    auto b = Kes::propertyBagFromJson(json, &g_errorHandler);

    auto out = Kes::propertyBagToJson(b);

    std::cout << out << "\n";

    b = Kes::propertyBagFromJson(const_cast<char*>(out.data()), &g_errorHandler);
    ASSERT_TRUE(b.isTable());
    auto& root = b.table();
    EXPECT_EQ(root.size(), 4);

    {
        auto it = root.find("IntArray");
        ASSERT_NE(it, root.end());
        ASSERT_TRUE(it->second->isArray());
        auto& intArray = it->second->array();
        EXPECT_EQ(intArray.size(), 3);

        {
            ASSERT_TRUE(intArray[0]->isProperty());
            EXPECT_EQ(std::any_cast<int>(intArray[0]->property().value), 1);
            ASSERT_TRUE(intArray[1]->isProperty());
            EXPECT_EQ(std::any_cast<int>(intArray[1]->property().value), 2);
            ASSERT_TRUE(intArray[2]->isProperty());
            EXPECT_EQ(std::any_cast<int>(intArray[2]->property().value), 3);
        }
    }

    {
        auto it = root.find("Table1");
        ASSERT_NE(it, root.end());
        ASSERT_TRUE(it->second->isTable());
        auto& table1 = it->second->table();
        EXPECT_EQ(table1.size(), 2);

        {
            auto it2 = table1.find("BoolProperty");
            ASSERT_NE(it2, table1.end());
            ASSERT_TRUE(it2->second->isProperty());
            EXPECT_EQ(std::any_cast<bool>(it2->second->property().value), true);
            it2 = table1.find("IntProperty");
            ASSERT_NE(it2, table1.end());
            ASSERT_TRUE(it2->second->isProperty());
            EXPECT_EQ(std::any_cast<int>(it2->second->property().value), 13);
        }
    }

    {
        auto it = root.find("Int64Property");
        ASSERT_NE(it, root.end());
        ASSERT_TRUE(it->second->isProperty());
        EXPECT_EQ(std::any_cast<int64_t>(it->second->property().value), -67);
    }

    {
        auto it = root.find("Table2");
        ASSERT_NE(it, root.end());
        ASSERT_TRUE(it->second->isTable());
        auto& table2 = it->second->table();
        EXPECT_EQ(table2.size(), 3);

        {
            auto it2 = table2.find("Table21");
            ASSERT_NE(it2, table2.end());
            ASSERT_TRUE(it2->second->isTable());
            auto& table21 = it2->second->table();
            EXPECT_EQ(table21.size(), 2);

            {
                auto it3 = table21.find("StringProperty");
                ASSERT_NE(it3, table21.end());
                ASSERT_TRUE(it3->second->isProperty());
                EXPECT_STREQ(std::any_cast<std::string>(it3->second->property().value).c_str(), "some text");

                it3 = table21.find("BoolProperty");
                ASSERT_NE(it3, table21.end());
                ASSERT_TRUE(it3->second->isProperty());
                EXPECT_EQ(std::any_cast<bool>(it3->second->property().value), false);
            }

            it2 = table2.find("IntArray");
            ASSERT_NE(it2, table2.end());
            ASSERT_TRUE(it2->second->isArray());
            auto& array = it2->second->array();
            EXPECT_EQ(array.size(), 4);

            {
                ASSERT_TRUE(array[0]->isProperty());
                EXPECT_EQ(std::any_cast<int>(array[0]->property().value), 0);
                ASSERT_TRUE(array[1]->isProperty());
                EXPECT_EQ(std::any_cast<int>(array[1]->property().value), 9);
                ASSERT_TRUE(array[2]->isProperty());
                EXPECT_EQ(std::any_cast<int>(array[2]->property().value), 8);
                ASSERT_TRUE(array[3]->isProperty());
                EXPECT_EQ(std::any_cast<int>(array[3]->property().value), 7);
            }

            it2 = table2.find("IntProperty");
            ASSERT_NE(it2, table2.end());
            ASSERT_TRUE(it2->second->isProperty());
            EXPECT_EQ(std::any_cast<int>(it2->second->property().value), -5);

        }
    }
}
