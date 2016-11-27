#pragma once
#include <functional>
#include <picojson.h>
#include <unordered_map>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

namespace PolyminisServer
{
    namespace JsonHelpers
    {
        std::string json_get_string(picojson::value& v, const std::string& fieldname);
        float json_get_float(picojson::value& v, const std::string& fieldname);

        const picojson::object& json_get_object(picojson::value& v, const std::string& fieldname);


        // ERRORS
        picojson::object json_create_error(const std::string& e_msg);
    }
}

namespace PolyminisServer
{
    typedef websocketpp::server<websocketpp::config::asio> ws_server_t;
    // pull out the type of messages sent by our config
    typedef ws_server_t::message_ptr message_ptr;
     
    typedef std::function<picojson::object(picojson::value&)> ws_service_handler_t;
    
    struct WSService
    {
      std::string mServiceName;
      ws_service_handler_t mHandler; 
    };
    
    class WSServer
    {
    public:
        WSServer()
        {
            std::shared_ptr<WSService> wss = std::make_shared<WSService>();
            wss->mServiceName = "control";
            wss->mHandler =  [=] (picojson::value& request)
                             {
                                 return this->ControlEndpoint(request);
                             };
            AddService(wss);
        }
        ~WSServer()
        {
            mServer.stop_listening();
        }
      
        void RunServer();
        void OnMessageReceived(websocketpp::connection_hdl hdl, message_ptr msg);
  
        void AddService(std::shared_ptr<WSService> service);
        void RemoveService(std::string serviceName);

        picojson::object ControlEndpoint(picojson::value& request);

        void SendMessage(picojson::value& message, websocketpp::connection_hdl& hdl, message_ptr msgptr);
    
    private:
        ws_server_t mServer;
        std::unordered_map<std::string, std::shared_ptr<WSService>> mServices;
    };
};
