#include "JsonHelpers.h"

namespace JsonHelpers
{
    const picojson::object Empty = picojson::object();
    const picojson::array  EmptyArray = picojson::array();
    // TODO: I think this should be done with templates, but those things scare the shit out of me
    std::string json_get_string(const picojson::value& v, const std::string& fieldname)
    {
        //check if the type of the value is "object"
        if (! v.is<picojson::object>())
        {
            // ERROR
            std::cout << "Error value is not an object (json_get_string)" << std::endl;
            return "";
        }
        // obtain a const reference to the map
        const picojson::value::object& obj = v.get<picojson::object>();
        auto it = obj.find(fieldname);
        if (it == obj.end())
        {
            // ERROR 
            std::cout << "Error field: "<< fieldname << " not found" << std::endl;
            return "";
        }
        
        // Use to_str to give ownership to the caller
        return it->second.to_str();
    }

    float json_get_float(const picojson::value& v, const std::string& fieldname)
    {
        //check if the type of the value is "object"
        if (! v.is<picojson::object>())
        {
            // ERROR
            std::cout << "Error value is not an object (json_get_float)" << std::endl;
            return 0.0f;
        }
        // obtain a const reference to the map
        const picojson::value::object& obj = v.get<picojson::object>();
        auto it = obj.find(fieldname);
        if (it == obj.end())
        {
           // ERROR 
           std::cout << "Error field: "<< fieldname << " not found" << std::endl;
           return 0.0f;
        }
        return (float) it->second.get<double>();
    }

    int json_get_int(const picojson::value& v, const std::string& fieldname)
    {
        //check if the type of the value is "object"
        if (! v.is<picojson::object>())
        {
            // ERROR
            std::cout << "Error value is not an object (json_get_int)" << std::endl;
            return 0;
        }
        // obtain a const reference to the map
        const picojson::value::object& obj = v.get<picojson::object>();
        auto it = obj.find(fieldname);
        if (it == obj.end())
        {
           // ERROR 
           std::cout << "Error field: "<< fieldname << " not found" << std::endl;
           return 0;
        }
        return (int)it->second.get<double>();
    }

    bool json_get_bool(const picojson::value& v, const std::string& fieldname)
    {
        //check if the type of the value is "object"
        if (! v.is<picojson::object>())
        {
            // ERROR
            std::cout << "Error value is not an object (json_get_int)" << std::endl;
            return false;
        }
        // obtain a const reference to the map
        const picojson::value::object& obj = v.get<picojson::object>();
        auto it = obj.find(fieldname);
        if (it == obj.end())
        {
           // ERROR 
           std::cout << "Error field: "<< fieldname << " not found" << std::endl;
           return false;
        }

        return (bool)it->second.get<bool>();
    }

    const picojson::object& json_get_object(const picojson::value& v, const std::string& fieldname)
    {
        if (! v.is<picojson::object>())
        {
            // ERROR
            std::cout << "Error value is not an object (json_get_object)" << std::endl;
            return Empty;
        }
        // obtain a const reference to the map
        const picojson::value::object& obj = v.get<picojson::object>();
        auto it = obj.find(fieldname);
        if (it == obj.end())
        {
           // ERROR 
           std::cout << "Error field: "<< fieldname << " not found" << std::endl;
           return Empty;
        }
        else
        {
            return it->second.get<picojson::object>();
        }
    }

    const picojson::object& json_get_as_object(const picojson::value& v)
    {
        if (! v.is<picojson::object>())
        {
            // ERROR
            std::cout << "Error value is not an object (json_get_as_object)" << std::endl;
            return Empty;
        }
        return v.get<picojson::object>();
    }


    float json_get_as_float(const picojson::value& v)
    {
        if (!v.is<double>())
        {
            std::cout << "Error value is not a float (json_get_as_float)" << std::endl;
            return 0.0;
        }
        return (float) v.get<double>();
    }


    std::string json_get_as_string(const picojson::value& v)
    {
        if (! v.is<std::string>())
        {
            // ERROR
            std::cout << "Error value is not a string (json_get_as_string)" << std::endl;
            return "";
        }
        return v.to_str();
    }


    const picojson::array& json_get_as_array(const picojson::value& v)
    {
        if (! v.is<picojson::array>() )
        {
            // ERROR
            std::cout << "Error value is not an object (json_get_as_array)" << std::endl;
            return EmptyArray;
        }
        return v.get<picojson::array>();
    }

    const picojson::array& json_get_array(const picojson::value& v, const std::string& fieldname)
    {
        if (! v.is<picojson::object>())
        {
            // ERROR
            std::cout << "Error value is not an object (json_get_array)" << std::endl;
            return EmptyArray;
        }
        // obtain a const reference to the map
        const picojson::value::object& obj = v.get<picojson::object>();
        auto it = obj.find(fieldname);
        if (it == obj.end())
        {
            // ERROR 
            std::cout << "Error field: "<< fieldname << " not found" << std::endl;
            return EmptyArray;
        }
        return it->second.get<picojson::array>();
    }

    bool json_has_field(const picojson::value& v, const std::string& fieldname)
    {
        if (! v.is<picojson::object>())
        {
            // ERROR
            std::cout << "Error value is not an object (json_has_field)" << std::endl;
            return false;
        }
        const picojson::value::object& obj = v.get<picojson::object>();
        return obj.count(fieldname) > 0;
    }

    picojson::object json_create_error(const std::string& e_msg)
    {
        
        std::cout << "Creating Error: " << e_msg << std::endl;
        picojson::object obj;
        obj["error"] = picojson::value(e_msg);
        return  obj;
    }
}
