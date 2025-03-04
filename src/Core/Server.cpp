#include "HttpClient.h"
#include "Server.h"
#include "JsonHelpers.h"

namespace PolyminisServer
{
    using websocketpp::lib::placeholders::_1;
    using websocketpp::lib::placeholders::_2;
    using websocketpp::lib::bind;


    void WSServer::RunServer()
    {
        // Set logging settings
        mServer.set_access_channels(websocketpp::log::alevel::all);
        mServer.clear_access_channels(websocketpp::log::alevel::frame_payload);
        // Initialize Asio
        mServer.init_asio();


        // Register Open / Close conn handlers
        mServer.set_open_handler(bind(&WSServer::OnConnectionOpen,this, _1));
        mServer.set_close_handler(bind(&WSServer::OnConnectionClose,this, _1));

        // Register our message handler
        mServer.set_message_handler(bind(&WSServer::OnMessageReceived, this, _1, _2));
        // TODO: Port shouldn't be hardcoded
        mServer.listen(8080);
        // Start the server accept loop
        mServer.start_accept();
        // Start the ASIO io_service run loop
        mServer.run();
    }

    void WSServer::OnConnectionOpen(websocketpp::connection_hdl hdl)
    {
        SessionData session;
        mConnections[hdl] = session;
    }

    void WSServer::OnMessageReceived(websocketpp::connection_hdl hdl, message_ptr msg) 
    {
        std::string message = msg->get_payload();
    
        std::cout << "Message received: " << message << std::endl;

        picojson::value v;
        std::string err = picojson::parse(v, message);
        if (!err.empty())        
        {
            // ERROR
            std::cout << "Error Parsing Message: " << message << std::endl;
        }

        picojson::object response;
        auto it = mServices.find(JsonHelpers::json_get_string(v, "Service"));
        if (it != mServices.end())
        {
            std::cout << "Service Found: " << message << std::endl;
            response = it->second->mHandler(v, mConnections[hdl]);
        }
        else
        {
            std::cout << "Service Not Found: " << message << std::endl;
        }

        if (!response.empty())
        {
            auto pv = picojson::value(response);
            SendMessage(pv, hdl, msg);
        }
    }

    void WSServer::OnConnectionClose(websocketpp::connection_hdl hdl)
    {
        mConnections.erase(hdl);
    }

    picojson::object WSServer::ControlEndpoint(picojson::value& request)
    {
        
        std::cout << "Request Content: " << request.to_str() << std::endl;
        std::string command = JsonHelpers::json_get_string(request, "Command");

        if (command == "STOP_LISTENING")
        {
            mServer.stop_listening();
        }
        else
        {
            // Got unrecognized command
        }

        return std::move(picojson::object());
    }

    void WSServer::AddService(std::shared_ptr<WSService> service)
    {
        mServices[service->mServiceName] = service;
    }
    void WSServer::RemoveService(std::string serviceName)
    {
        mServices.erase(serviceName);
    }

    void WSServer::SendMessage(picojson::value& message, websocketpp::connection_hdl& hdl, message_ptr msgptr)
    {
        try
        {
             auto message_str = message.serialize();
             mServer.send(hdl, message_str, msgptr->get_opcode());
        }
        catch (const websocketpp::lib::error_code& e)
        {
            std::cout << "Send Message Failed: " << e << "(" << e.message() << ")" << std::endl;
        }
    } 
}
