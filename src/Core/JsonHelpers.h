#include <iostream>
#include <picojson.h>
namespace JsonHelpers
{
    std::string json_get_string(picojson::value& v, const std::string& fieldname);
    float json_get_float(picojson::value& v, const std::string& fieldname);
    int json_get_int(picojson::value& v, const std::string& fieldname);

    const picojson::object& json_get_object(picojson::value& v, const std::string& fieldname);
    const picojson::array& json_get_array(picojson::value& v, const std::string& fieldname);

    const picojson::object& json_get_as_object(picojson::value& v);

    bool json_has_field(picojson::value& v, const std::string& fieldname);

    // ERRORS
    picojson::object json_create_error(const std::string& e_msg);
}
