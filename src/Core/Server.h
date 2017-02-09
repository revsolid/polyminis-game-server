#pragma once
#include <functional>
#include <picojson.h>
#include <unordered_map>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

namespace PolyminisServer
{
    struct ServerCfg
    {
        std::string host;
        int port;
        ServerCfg(std::string host, int port) :
	host(host),
        port(port)
        {}
    };

    typedef websocketpp::server<websocketpp::config::asio> ws_server_t;
    // pull out the type of messages sent by our config
    typedef ws_server_t::message_ptr message_ptr;
     
    typedef std::function<picojson::object(picojson::value&)> ws_service_handler_t;

    struct WSService
    {
      std::string mServiceName;
      ws_service_handler_t mHandler; 
    };

    struct SessionData
    {
        bool b;
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
        void OnConnectionOpen(websocketpp::connection_hdl hdl);
        void OnConnectionClose(websocketpp::connection_hdl hdl);
        void OnMessageReceived(websocketpp::connection_hdl hdl, message_ptr msg);
  
        void AddService(std::shared_ptr<WSService> service);
        void RemoveService(std::string serviceName);

        void SendMessage(picojson::value& message, websocketpp::connection_hdl& hdl, message_ptr msgptr);

        picojson::object ControlEndpoint(picojson::value& request);
    
    private:

        // Members
 
        // Data to keep per Session
        std::unordered_map<websocketpp::connection_hdl, SessionData,
                           std::owner_less<websocketpp::connection_hdl>> mConnections;
        ws_server_t mServer;
        std::unordered_map<std::string, std::shared_ptr<WSService>> mServices;
    };
};
