#include <beast/http.hpp>
#include <beast/core/to_string.hpp>
#include <boost/asio.hpp>
#include <string>
#include "HttpClient.h"

using namespace PolyminisServer;


std::string HttpMethodToString(HttpMethod method)
{
    std::string toRet = "GET";
    switch(method)
    {
    case HttpMethod::POST:
        toRet = "POST";
        break;

    case HttpMethod::PUT:
        toRet = "PUT";
        break;

    case HttpMethod::DELETE:
        toRet = "DELETE";
        break;

    case HttpMethod::GET:
    // Fallthrough
    default:
        toRet = "GET";
        break;
    }

    return toRet;
}



picojson::object HttpClient::Request(const std::string& host, int port,
                                     const std::string& url, HttpMethod method,
                                     const picojson::object& payload)
{
    boost::asio::io_service ios;
    boost::asio::ip::tcp::resolver r{ios};
    boost::asio::ip::tcp::socket sock{ios};
    std::cout << "HttpClient::Connecting to " << host << ":" << port << url << " (" << HttpMethodToString(method) << ")"<< std::endl;
    try
    {
        boost::asio::connect(sock,
            r.resolve(boost::asio::ip::tcp::resolver::query{host, std::to_string(port)}));
    }
    catch (std::exception const & e) 
    {
        std::cout << e.what() << std::endl;
        return std::move(picojson::object());
    }
    std::cout << "  Done.." << std::endl;

    beast::http::request<beast::http::string_body> req;

    req.method = HttpMethodToString(method); 
    req.url = std::string(url);
    req.version = 11;

    req.body = picojson::value(payload).serialize();
    req.fields.insert("Content-Type", "application/json");

    beast::http::prepare(req);
    beast::http::write(sock, req);

    beast::streambuf sb;
    beast::http::response<beast::http::streambuf_body> resp;
    beast::http::read(sock, sb, resp);

    picojson::value v;
    std::string body = to_string(resp.body.data());
    picojson::object obj;
    obj["Status"] = picojson::value((double)resp.status);

    std::string err = picojson::parse(v, body);
    if (!err.empty())        
    {
        // ERROR
        std::cout << "HttpClient - Error Parsing Message Body: " << body << std::endl;
        return std::move(obj);
    }
    obj["Response"] = std::move(v);
    return std::move(obj);
}
