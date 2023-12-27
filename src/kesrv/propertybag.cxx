#include <kesrv/knownprops.hxx>
#include <kesrv/propertybag.hxx>
#include <kesrv/util/format.hxx>

namespace Kes
{

Property propertyFromJson(const char* name, const Json::Value& v, Log::ILog* log)
{
    if (v.IsArray())
    {
        if (log)
            log->write(Log::Level::Warning, "Property %s is an array while a scalar expected", name);
        
        return Property();
    }

    if (v.IsObject())
    {
        if (log)
            log->write(Log::Level::Warning, "Property %s is an object while a scalar expected", name);
        
        return Property();
    }

    auto info = lookupProperty(name);
    if (!info)
    {
        if (log)
            log->write(Log::Level::Warning, "Unsupported property %s", name);
        
        return Property();
    }

    auto id = info->id();
    auto& type = info->type();

    if (type == typeid(bool))
    {
        if (!v.IsBool())
        {
            if (log)
                log->write(Log::Level::Warning, "Property %s is not a \'bool\'", name);
        
            return Property();
        }

        return Property(id, v.GetBool(), info);
    }
    else if (type == typeid(int))
    {
        if (!v.IsInt())
        {
            if (log)
                log->write(Log::Level::Warning, "Property %s is not an \'int\'", name);
        
            return Property();
        }

        return Property(id, v.GetInt(), info);
    }
    else if (type == typeid(int64_t))
    {
        if (!v.IsInt64())
        {
            if (log)
                log->write(Log::Level::Warning, "Property %s is not an \'int64_t\'", name);
        
            return Property();
        }

        return Property(id, v.GetInt64(), info);
    }
    else if (type == typeid(unsigned int))
    {
        if (!v.IsUint())
        {
            if (log)
                log->write(Log::Level::Warning, "Property %s is not an \'unsigned int\'", name);
        
            return Property();
        }

        return Property(id, v.GetUint(), info);
    }
    else if (type == typeid(uint64_t))
    {
        if (!v.IsUint64())
        {
            if (log)
                log->write(Log::Level::Warning, "Property %s is not an \'uint64_t\'", name);
        
            return Property();
        }

        return Property(id, v.GetUint64(), info);
    }
    else if (type == typeid(double))
    {
        if (!v.IsDouble())
        {
            if (log)
                log->write(Log::Level::Warning, "Property %s is not a \'double\'", name);
        
            return Property();
        }

        return Property(id, v.GetDouble(), info);
    }
    else if (type == typeid(std::string))
    {
        if (!v.IsString())
        {
            if (log)
                log->write(Log::Level::Warning, "Property %s is not an \'std::string\'", name);
        
            return Property();
        }

        return Property(id, std::string(v.GetString(), v.GetStringLength()), info);
    }

    if (log)
        log->write(Log::Level::Warning, "Property %s is of an unsupported type \'%s\'", name, type.name());
        
    return Property();
}

} // namespace Kes {}