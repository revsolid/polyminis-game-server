#pragma once
#include <picojson.h>


namespace PolyminisServer
{
    enum HttpMethod
    {
        GET,
        POST
    };
    struct HttpClient
    {
        static const picojson::object& Request(const std::string& host, int port, HttpMethod method, const picojson::object& payload);
    }; 
}
