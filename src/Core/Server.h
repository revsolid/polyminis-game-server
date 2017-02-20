#pragma once
#include <functional>
#include <picojson.h>
#include <unordered_map>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include "Core/Types.h"

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
    typedef ws_server_t::message_ptr message_ptr;

    // NOTE: There is a bit of a layering violation as the SessionData is carrying game-specific data,
    // this should be easy to solve with templates or inheritance (TODO)
    struct SessionData
    {
        // int SimServerID 
        
        std::string UserName;

        float       BiomassAvailable = 100.0f;
        float       VisibilityRange = 600.0f;
        Coord       Position; 
    };
    

    typedef std::function<picojson::object(picojson::value&, SessionData&)> ws_service_handler_t;

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
            wss->mHandler =  [=] (picojson::value& request, SessionData& sd)
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
        std::map<websocketpp::connection_hdl, SessionData, std::owner_less<websocketpp::connection_hdl>> mConnections;

       // std::map<connection_hdl,connection_data,std::owner_less<connection_hdl>> con_list;

        ws_server_t mServer;
        std::unordered_map<std::string, std::shared_ptr<WSService>> mServices;
    };
};
