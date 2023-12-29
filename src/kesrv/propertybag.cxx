#include <kesrv/exception.hxx>
#include <kesrv/json.hxx>
#include <kesrv/knownprops.hxx>
#include <kesrv/propertybag.hxx>
#include <kesrv/util/format.hxx>

namespace Kes
{

static PropertyBag propertyBagFromJsonValue(const char* name, const Json::Value& v, IPropertyErrorHandler* eh);

static void handleError(SourceLocation where, IPropertyErrorHandler* eh, const char* format, ...)
{
    assert(eh);

    va_list args;
    va_start(args, format);
    auto message = Util::formatv(format, args);
    va_end(args);

    auto action = eh->handle(where, message);
    if (action == CallbackResult::Abort)
        throw Exception(where, std::move(message));
}


static Property propertyFromJsonValue(const char* name, const Json::Value& v, IPropertyErrorHandler* eh)
{
    if (v.IsArray())
    {
        handleError(KES_HERE(), eh, "Property \'%s\' is an array while a scalar expected", name);
        return Property();
    }

    if (v.IsObject())
    {
        handleError(KES_HERE(), eh, "Property \'%s\' is an object while a scalar expected", name);
        return Property();
    }

    auto info = lookupProperty(name);
    if (!info)
    {
        handleError(KES_HERE(), eh, "Unsupported property \'%s\'", name);
        return Property();
    }

    auto id = info->id();
    auto& type = info->type();

    if (type == typeid(bool))
    {
        if (!v.IsBool())
        {
            handleError(KES_HERE(), eh, "Property \'%s\' is not a \'bool\'", name);
            return Property();
        }

        return Property(id, v.GetBool(), info);
    }
    else if (type == typeid(int))
    {
        if (!v.IsInt())
        {
            handleError(KES_HERE(), eh, "Property \'%s\' is not an \'int\'", name);
            return Property();
        }

        return Property(id, v.GetInt(), info);
    }
    else if (type == typeid(int64_t))
    {
        if (!v.IsInt64())
        {
            handleError(KES_HERE(), eh, "Property \'%s\' is not an \'int64_t\'", name);
            return Property();
        }

        return Property(id, v.GetInt64(), info);
    }
    else if (type == typeid(unsigned int))
    {
        if (!v.IsUint())
        {
            handleError(KES_HERE(), eh, "Property \'%s\' is not an \'unsigned int\'", name);
            return Property();
        }

        return Property(id, v.GetUint(), info);
    }
    else if (type == typeid(uint64_t))
    {
        if (!v.IsUint64())
        {
            handleError(KES_HERE(), eh, "Property \'%s\' is not an \'uint64_t\'", name);
            return Property();
        }

        return Property(id, v.GetUint64(), info);
    }
    else if (type == typeid(double))
    {
        if (!v.IsDouble())
        {
            handleError(KES_HERE(), eh, "Property \'%s\' is not a \'double\'", name);
            return Property();
        }

        return Property(id, v.GetDouble(), info);
    }
    else if (type == typeid(std::string))
    {
        if (!v.IsString())
        {
            handleError(KES_HERE(), eh, "Property \'%s\' is not an \'std::string\'", name);
            return Property();
        }

        return Property(id, std::string(v.GetString(), v.GetStringLength()), info);
    }

    handleError(KES_HERE(), eh, "Property \'%s\' is of an unsupported type \'%s\'", name, type.name());

    return Property();
}

static PropertyBag arrayItemFromJson(const char* arrayName, const std::type_info& type, size_t index, const Json::Value& v, IPropertyErrorHandler* eh)
{
    if (v.IsArray())
    {
        // nested arrays are not supported
        handleError(KES_HERE(), eh, "Array \'%s\' item #%zu is an array while a scalar expected", arrayName, index);
        return PropertyBag();
    }

    if (v.IsObject())
    {
        return propertyBagFromJsonValue("", v, eh);
    }

    if (type == typeid(bool))
    {
        if (!v.IsBool())
        {
            handleError(KES_HERE(), eh, "Array \'%s\' item #%zu is not a \'bool\'", arrayName, index);
            return PropertyBag();
        }

        return PropertyBag("", Property(InvalidPropId, v.GetBool()));
    }
    else if (type == typeid(int))
    {
        if (!v.IsInt())
        {
            handleError(KES_HERE(), eh, "Array \'%s\' item #%zu is not an \'int\'", arrayName, index);
            return PropertyBag();
        }

        return PropertyBag("", Property(InvalidPropId, v.GetInt()));
    }
    else if (type == typeid(int64_t))
    {
        if (!v.IsInt64())
        {
            handleError(KES_HERE(), eh, "Array \'%s\' item #%zu is not an \'int64_t\'", arrayName, index);
            return PropertyBag();
        }

        return PropertyBag("", Property(InvalidPropId, v.GetInt64()));
    }
    else if (type == typeid(unsigned int))
    {
        if (!v.IsUint())
        {
            handleError(KES_HERE(), eh, "Array \'%s\' item #%zu is not an \'unsigned int\'", arrayName, index);
            return PropertyBag();
        }

        return PropertyBag("", Property(InvalidPropId, v.GetUint()));
    }
    else if (type == typeid(uint64_t))
    {
        if (!v.IsUint64())
        {
            handleError(KES_HERE(), eh, "Array \'%s\' item #%zu is not an \'uint64_t\'", arrayName, index);
            return PropertyBag();
        }

        return PropertyBag("", Property(InvalidPropId, v.GetUint64()));
    }
    else if (type == typeid(double))
    {
        if (!v.IsDouble())
        {
            handleError(KES_HERE(), eh, "Array \'%s\' item #%zu is not an \'double\'", arrayName, index);
            return PropertyBag();
        }

        return PropertyBag("", Property(InvalidPropId, v.GetDouble()));
    }
    else if (type == typeid(std::string))
    {
        if (!v.IsString())
        {
            handleError(KES_HERE(), eh, "Array \'%s\' item #%zu is not an \'std::string\'", arrayName, index);
            return PropertyBag();
        }

        return PropertyBag("", Property(InvalidPropId, std::string(v.GetString(), v.GetStringLength())));
    }

    handleError(KES_HERE(), eh, "Array \'%s\' item #%zu is of an unsupported type \'%s\'", arrayName, index, type.name());
    return PropertyBag();
}

static PropertyBag::Array arrayFromJsonValue(const char* name, const Json::Value& v, IPropertyErrorHandler* eh)
{
    auto info = lookupProperty(name);
    if (!info)
    {
        handleError(KES_HERE(), eh, "Unsupported property \'%s\'", name);
        return PropertyBag::Array();
    }

    auto& type = info->type();
    if (type != typeid(PropertyBag::Array))
    {
        handleError(KES_HERE(), eh, "Property \'%s\' is not a \'PropertyBag::Array\'", name);
        return PropertyBag::Array();
    }

    if (!v.IsArray())
    {
        handleError(KES_HERE(), eh, "Property \'%s\' is not an array", name);
        return PropertyBag::Array();
    }

    auto& baseType = info->base();

    PropertyBag::Array array;

    for (size_t index = 0; index < v.Size(); ++index)
    {
        auto item = arrayItemFromJson(name, baseType, index, v[index], eh);
        array.push_back(std::make_unique<PropertyBag>(std::move(item)));
    }

    return array;
}

static PropertyBag::Table tableFromJsonValue(const char* name, const Json::Value& v, IPropertyErrorHandler* eh)
{
    if (!v.IsObject())
    {
        handleError(KES_HERE(), eh, "Property \'%s\' is not an object", name);
        return PropertyBag::Table();
    }

    PropertyBag::Table table;
    for (auto m = v.MemberBegin(); m != v.MemberEnd(); ++m)
    {
        std::string memberName(m->name.GetString(), m->name.GetStringLength());
        auto member = propertyBagFromJsonValue(memberName.c_str(), m->value, eh);

        table.insert({ memberName, std::make_unique<PropertyBag>(std::move(member)) });
    }

    return table;
}

static PropertyBag propertyBagFromJsonValue(const char* name, const Json::Value& v, IPropertyErrorHandler* eh)
{
    if (v.IsObject())
        return PropertyBag(name, tableFromJsonValue(name, v, eh));

    if (v.IsArray())
        return PropertyBag(name, arrayFromJsonValue(name, v, eh));

    return PropertyBag(name, propertyFromJsonValue(name, v, eh));
}


KESRV_EXPORT PropertyBag propertyBagFromJson(char* json, IPropertyErrorHandler* eh)
{
    Kes::Json::Document doc;
    doc.ParseInsitu(json);
    if (doc.HasParseError())
    {
        auto err = doc.GetParseError();
        throw Exception(KES_HERE(), Util::format("Failed to parse JSON: [%s] at %zu", Json::GetParseError_En(err), doc.GetErrorOffset()));
    }

    return propertyBagFromJsonValue("", doc, eh);
}


static void propertyBagToJson(const PropertyBag& bag, Json::Writer<Json::StringBuffer>& writer);

static void propertyToJson(const PropertyBag& bag, Json::Writer<Json::StringBuffer>& writer)
{
    assert(!bag.isEmpty());
    assert(!bag.isArray());
    assert(!bag.isTable());
    assert(bag.isProperty());

    auto& prop = bag.property();
    assert(prop.value.has_value());
    if (!prop.value.has_value())
        throw Exception(KES_HERE(), Util::format("Property %08x has no value", prop.id));

    const std::type_info* type = nullptr;

    auto info = prop.info;
    if (!info)
        info = Kes::lookupProperty(prop.id);

    if (info)
        type = &info->type();
    else
        type = &prop.value.type();

    if (*type == typeid(bool))
    {
        writer.Bool(std::any_cast<bool>(prop.value));
    }
    else if (*type == typeid(int))
    {
        writer.Int(std::any_cast<int>(prop.value));
    }
    else if (*type == typeid(unsigned int))
    {
        writer.Uint(std::any_cast<unsigned int>(prop.value));
    }
    else if (*type == typeid(int64_t))
    {
        writer.Int64(std::any_cast<int64_t>(prop.value));
    }
    else if (*type == typeid(uint64_t))
    {
        writer.Uint64(std::any_cast<uint64_t>(prop.value));
    }
    else if (*type == typeid(std::string))
    {
        auto s = std::any_cast<std::string>(prop.value);
        writer.String(s.data(), s.length());
    }
}

static void tableToJson(const PropertyBag& bag, Json::Writer<Json::StringBuffer>& writer)
{
    assert(bag.isTable());
    auto& t = bag.table();

    writer.StartObject();

    for (auto& prop: t)
    {
        if (!prop.second->isEmpty())
        {
            assert(prop.first == prop.second->name());
            propertyBagToJson(*prop.second, writer);
        }
    }

    writer.EndObject();
}

static void arrayToJson(const PropertyBag& bag, Json::Writer<Json::StringBuffer>& writer)
{
    assert(bag.isArray());
    auto& a = bag.array();

    writer.StartArray();

    for (auto& prop: a)
    {
        if (!prop->isEmpty())
        {
            propertyBagToJson(*prop, writer);
        }
    }

    writer.EndArray();
}

static void propertyBagToJson(const PropertyBag& bag, Json::Writer<Json::StringBuffer>& writer)
{
    if (!bag.name().empty())
        writer.Key(bag.name().data(), bag.name().length());

    if (bag.isArray())
        arrayToJson(bag, writer);
    else if (bag.isTable())
        tableToJson(bag, writer);
    else
        propertyToJson(bag, writer);
}

KESRV_EXPORT std::string propertyBagToJson(const PropertyBag& bag)
{
    Json::StringBuffer sb;
    Json::Writer<Json::StringBuffer> writer(sb);

    propertyBagToJson(bag, writer);

    return std::string(sb.GetString());
}

} // namespace Kes {}
