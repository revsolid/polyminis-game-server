#include <beast/http.hpp>
#include <beast/core/to_string.hpp>
#include <boost/asio.hpp>
#include <string>
#include "HttpClient.h"

using namespace PolyminisServer;
picojson::object HttpClient::Request(const std::string& host, int port,
                                     const std::string& url, HttpMethod method,
                                     const picojson::object& payload)
{
    boost::asio::io_service ios;
    boost::asio::ip::tcp::resolver r{ios};
    boost::asio::ip::tcp::socket sock{ios};
    std::cout << "HttpClient::Connecting to " << host << ":" << port << url << std::endl;
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

    beast::http::request<beast::http::empty_body> req;

    switch(method)
    {
    case HttpMethod::POST:
        req.method = "POST";
        break;
    case HttpMethod::GET:
    // Fallthrough
    default:
        req.method = "GET";
        break;
    }

    req.url = std::string(url);
    req.version = 11;
    beast::http::prepare(req);
    beast::http::write(sock, req);

    beast::streambuf sb;
    beast::http::response<beast::http::streambuf_body> resp;
    beast::http::read(sock, sb, resp);
    std::cout << resp << std::endl;

    picojson::value v;
    std::string body = to_string(resp.body.data());
    std::string err = picojson::parse(v, body);
    if (!err.empty())        
    {
        // ERROR
        std::cout << "Error Parsing Message: " << body << std::endl;
        return std::move(picojson::object());
    }
    picojson::object obj;
    obj["Response"] = std::move(v);
    return std::move(obj);
}
