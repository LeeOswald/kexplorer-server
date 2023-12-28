#include <kesrv/exception.hxx>
#include <kesrv/knownprops.hxx>
#include <kesrv/propertybag.hxx>
#include <kesrv/util/format.hxx>

namespace Kes
{

static void handleError(SourceLocation where, IPropertyErrorHandler* eh, const char* format, ...)
{
    assert(eh);

    va_list args;
    va_start(args, format);
    auto message = Util::formatv(format, args);
    va_end(args);

    auto action = eh->handle(where, message);
    if (action == IPropertyErrorHandler::Result::Throw)
        throw Exception(where, std::move(message));
}


KESRV_EXPORT Property propertyFromJson(const char* name, const Json::Value& v, IPropertyErrorHandler* eh)
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
        return propertyBagFromJson("", v, eh);
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

KESRV_EXPORT PropertyBag::Array arrayFromJson(const char* name, const Json::Value& v, IPropertyErrorHandler* eh)
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

KESRV_EXPORT PropertyBag::Table tableFromJson(const char* name, const Json::Value& v, IPropertyErrorHandler* eh)
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
        auto member = propertyBagFromJson(memberName.c_str(), m->value, eh);
        
        table.insert({ memberName, std::make_unique<PropertyBag>(std::move(member)) });
    }

    return table;
}

KESRV_EXPORT PropertyBag propertyBagFromJson(const char* name, const Json::Value& v, IPropertyErrorHandler* eh)
{
    if (v.IsObject())
        return PropertyBag(name, tableFromJson(name, v, eh));

    if (v.IsArray())
        return PropertyBag(name, arrayFromJson(name, v, eh));

    return PropertyBag(name, propertyFromJson(name, v, eh));
}

} // namespace Kes {}
