#include <boost/asio.hpp>
#include <beast/http.hpp>
#include "HttpClient.h"

using namespace PolyminisServer;
const picojson::object& HttpClient::Request(const std::string& host_, int port, HttpMethod method, const picojson::object& payload)
{
    std::string const host = "github.com";
    boost::asio::io_service ios;
    boost::asio::ip::tcp::resolver r{ios};
    boost::asio::ip::tcp::socket sock{ios};
    boost::asio::connect(sock,
        r.resolve(boost::asio::ip::tcp::resolver::query{host, "http"}));

    beast::http::request<beast::http::empty_body> req;
    req.method = "GET";
    req.url = "/";
    req.version = 11;
    beast::http::prepare(req);
    beast::http::write(sock, req);

    beast::streambuf sb;
    beast::http::response<beast::http::streambuf_body> resp;
    std::cout << resp;

    picojson::object obj;
    return std::move(obj);
}

