#include "JsonHelpers.h"
namespace JsonHelpers
{
    // TODO: I think this should be done with templates, but those things scare the shit out of me
    std::string json_get_string(picojson::value& v, const std::string& fieldname)
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

    float json_get_float(picojson::value& v, const std::string& fieldname)
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

    int json_get_int(picojson::value& v, const std::string& filedname)
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
        return it->second.get<int>();
    }

    const picojson::object& json_get_object(picojson::value& v, const std::string& fieldname)
    {
        if (! v.is<picojson::object>())
        {
            // ERROR
            std::cout << "Error value is not an object (json_get_object)" << std::endl;
            return std::move(picojson::object());
        }
        // obtain a const reference to the map
        const picojson::value::object& obj = v.get<picojson::object>();
        auto it = obj.find(fieldname);
        if (it == obj.end())
        {
           // ERROR 
           std::cout << "Error field: "<< fieldname << " not found" << std::endl;
           return std::move(picojson::object());
        }
        return it->second.get<picojson::object>();
    }

    const picojson::array& json_get_array(picojson::value& v, const std::string& fieldname)
    {
        if (! v.is<picojson::object>())
        {
            // ERROR
            std::cout << "Error value is not an object (json_get_array)" << std::endl;
            return std::move(picojson::array());
        }
        // obtain a const reference to the map
        const picojson::value::object& obj = v.get<picojson::object>();
        auto it = obj.find(fieldname);
        if (it == obj.end())
        {
            // ERROR 
            std::cout << "Error field: "<< fieldname << " not found" << std::endl;
            return std::move(picojson::array());
        }
        return it->second.get<picojson::array>();
    }

    picojson::object json_create_error(const std::string& e_msg)
    {
        
        std::cout << "Creating Error: " << e_msg << std::endl;
        picojson::object obj;
        obj["error"] = picojson::value(e_msg);
        return  obj;
    }
}
