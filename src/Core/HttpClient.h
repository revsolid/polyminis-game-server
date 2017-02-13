#pragma once
#include <picojson.h>


namespace PolyminisServer
{
    enum HttpMethod
    {
        GET,
        POST,
        PUT,
        DELETE
    };
    struct HttpClient
    {
        static picojson::object Request(const std::string& host, int port, const std::string& url,
			                HttpMethod method, const picojson::object& payload);
    }; 
}
