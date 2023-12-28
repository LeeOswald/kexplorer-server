#include <kesrv/knownprops.hxx>
#include <kesrv/propertybag.hxx>
#include <kesrv/util/format.hxx>

namespace Kes
{

KESRV_EXPORT Property propertyFromJson(const char* name, const Json::Value& v, Log::ILog* log)
{
    if (v.IsArray())
    {
        LogWarning(log, "Property \'%s\' is an array while a scalar expected", name);
        return Property();
    }

    if (v.IsObject())
    {
        LogWarning(log, "Property \'%s\' is an object while a scalar expected", name);
        return Property();
    }

    auto info = lookupProperty(name);
    if (!info)
    {
        LogWarning(log, "Unsupported property \'%s\'", name);
        return Property();
    }

    auto id = info->id();
    auto& type = info->type();

    if (type == typeid(bool))
    {
        if (!v.IsBool())
        {
            LogWarning(log, "Property \'%s\' is not a \'bool\'", name);
            return Property();
        }

        return Property(id, v.GetBool(), info);
    }
    else if (type == typeid(int))
    {
        if (!v.IsInt())
        {
            LogWarning(log, "Property \'%s\' is not an \'int\'", name);
            return Property();
        }

        return Property(id, v.GetInt(), info);
    }
    else if (type == typeid(int64_t))
    {
        if (!v.IsInt64())
        {
            LogWarning(log, "Property \'%s\' is not an \'int64_t\'", name);
            return Property();
        }

        return Property(id, v.GetInt64(), info);
    }
    else if (type == typeid(unsigned int))
    {
        if (!v.IsUint())
        {
            LogWarning(log, "Property \'%s\' is not an \'unsigned int\'", name);
            return Property();
        }

        return Property(id, v.GetUint(), info);
    }
    else if (type == typeid(uint64_t))
    {
        if (!v.IsUint64())
        {
            LogWarning(log, "Property \'%s\' is not an \'uint64_t\'", name);
            return Property();
        }

        return Property(id, v.GetUint64(), info);
    }
    else if (type == typeid(double))
    {
        if (!v.IsDouble())
        {
            LogWarning(log, "Property \'%s\' is not a \'double\'", name);
            return Property();
        }

        return Property(id, v.GetDouble(), info);
    }
    else if (type == typeid(std::string))
    {
        if (!v.IsString())
        {
            LogWarning(log, "Property \'%s\' is not an \'std::string\'", name);
            return Property();
        }

        return Property(id, std::string(v.GetString(), v.GetStringLength()), info);
    }

    LogWarning(log, "Property \'%s\' is of an unsupported type \'%s\'", name, type.name());
        
    return Property();
}

static PropertyBag arrayItemFromJson(const char* arrayName, const std::type_info& type, size_t index, const Json::Value& v, Log::ILog* log)
{
    if (v.IsArray())
    {
        // nested arrays are not supported
        LogWarning(log, "Array \'%s\' item #%zu is an array while a scalar expected", arrayName, index);
        return PropertyBag();
    }

    if (v.IsObject())
    {
        return propertyBagFromJson("", v, log);
    }

    if (type == typeid(bool))
    {
        if (!v.IsBool())
        {
            LogWarning(log, "Array \'%s\' item #%zu is not a \'bool\'", arrayName, index);
            return PropertyBag();
        }

        return PropertyBag("", Property(InvalidPropId, v.GetBool()));
    }
    else if (type == typeid(int))
    {
        if (!v.IsInt())
        {
            LogWarning(log, "Array \'%s\' item #%zu is not an \'int\'", arrayName, index);
            return PropertyBag();
        }

        return PropertyBag("", Property(InvalidPropId, v.GetInt()));
    }
    else if (type == typeid(int64_t))
    {
        if (!v.IsInt64())
        {
            LogWarning(log, "Array \'%s\' item #%zu is not an \'int64_t\'", arrayName, index);
            return PropertyBag();
        }

        return PropertyBag("", Property(InvalidPropId, v.GetInt64()));
    }
    else if (type == typeid(unsigned int))
    {
        if (!v.IsUint())
        {
            LogWarning(log, "Array \'%s\' item #%zu is not an \'unsigned int\'", arrayName, index);
            return PropertyBag();
        }

        return PropertyBag("", Property(InvalidPropId, v.GetUint()));
    }
    else if (type == typeid(uint64_t))
    {
        if (!v.IsUint64())
        {
            LogWarning(log, "Array \'%s\' item #%zu is not an \'uint64_t\'", arrayName, index);
            return PropertyBag();
        }

        return PropertyBag("", Property(InvalidPropId, v.GetUint64()));
    }
    else if (type == typeid(double))
    {
        if (!v.IsDouble())
        {
            LogWarning(log, "Array \'%s\' item #%zu is not an \'double\'", arrayName, index);
            return PropertyBag();
        }

        return PropertyBag("", Property(InvalidPropId, v.GetDouble()));
    }
    else if (type == typeid(std::string))
    {
        if (!v.IsString())
        {
            LogWarning(log, "Array \'%s\' item #%zu is not an \'std::string\'", arrayName, index);
            return PropertyBag();
        }

        return PropertyBag("", Property(InvalidPropId, std::string(v.GetString(), v.GetStringLength())));
    }

    LogWarning(log, "Array \'%s\' item #%zu is of an unsupported type \'%s\'", arrayName, index, type.name());
    return PropertyBag();
}

KESRV_EXPORT PropertyBag::Array arrayFromJson(const char* name, const Json::Value& v, Log::ILog* log)
{
    auto info = lookupProperty(name);
    if (!info)
    {
        LogWarning(log, "Unsupported property \'%s\'", name);
        return PropertyBag::Array();
    }

    auto& type = info->type();
    if (type != typeid(PropertyBag::Array))
    {
        LogError(log, "Property \'%s\' is not a \'PropertyBag::Array\'", name);
        return PropertyBag::Array();
    } 

    if (!v.IsArray())
    {
        LogWarning(log, "Property \'%s\' is not an array", name);
        return PropertyBag::Array();
    }

    auto& baseType = info->base();

    PropertyBag::Array array;
    
    for (size_t index = 0; index < v.Size(); ++index)
    {
        auto item = arrayItemFromJson(name, baseType, index, v[index], log);
        array.push_back(std::make_unique<PropertyBag>(std::move(item)));
    }
    
    return array;
}

KESRV_EXPORT PropertyBag::Table tableFromJson(const char* name, const Json::Value& v, Log::ILog* log)
{
    if (!v.IsObject())
    {
        LogWarning(log, "Property \'%s\' is not an object", name);
        return PropertyBag::Table();
    }

    PropertyBag::Table table;
    for (auto m = v.MemberBegin(); m != v.MemberEnd(); ++m)
    {
        std::string memberName(m->name.GetString(), m->name.GetStringLength());
        auto member = propertyBagFromJson(memberName.c_str(), m->value, log);
        
        table.insert({ memberName, std::make_unique<PropertyBag>(std::move(member)) });
    }

    return table;
}

KESRV_EXPORT PropertyBag propertyBagFromJson(const char* name, const Json::Value& v, Log::ILog* log)
{
    if (v.IsObject())
        return PropertyBag(name, tableFromJson(name, v, log));

    if (v.IsArray())
        return PropertyBag(name, arrayFromJson(name, v, log));

    return PropertyBag(name, propertyFromJson(name, v, log));
}

} // namespace Kes {}