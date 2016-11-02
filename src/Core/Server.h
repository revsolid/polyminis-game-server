#pragma once
#include <functional>
#include <picojson.h>
#include <unordered_map>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>


namespace PolyminisServer
{
    typedef websocketpp::server<websocketpp::config::asio> ws_server_t;
    // pull out the type of messages sent by our config
    typedef ws_server_t::message_ptr message_ptr;
     
    typedef std::function<std::string(picojson::value&)> ws_service_handler_t;
    
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
        ~WSServer(){}
      
        void RunServer();
        void OnMessageReceived(websocketpp::connection_hdl hdl, message_ptr msg);
  
        void AddService(std::shared_ptr<WSService> service);
        void RemoveService(std::string serviceName);

        std::string ControlEndpoint(picojson::value& command){}
    
    private:
        ws_server_t mServer;
        std::unordered_map<std::string, std::shared_ptr<WSService>> mServices;
    };
};
