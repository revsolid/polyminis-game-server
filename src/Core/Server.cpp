#include "Server.h"


static std::string json_get_string(picojson::value& v, const std::string& fieldname)
{
    //check if the type of the value is "object"
    if (! v.is<picojson::object>())
    {
        // ERROR
    }
    // obtain a const reference to the map, and print the contents
    const picojson::value::object& obj = v.get<picojson::object>();
    auto it = obj.find(fieldname);
    if (it == obj.end())
    {
       // ERROR 
    }
    
    std::move(it->second.to_str());
}

namespace PolyminisServer
{
    using websocketpp::lib::placeholders::_1;
    using websocketpp::lib::placeholders::_2;
    using websocketpp::lib::bind;

    void WSServer::OnMessageReceived(websocketpp::connection_hdl hdl, message_ptr msg) 
    {
        std::string message = msg->get_payload();
    
        std::cout << "Message received: " << message << std::endl;
        picojson::value v;
        std::string err = picojson::parse(v, message);

        if (!err.empty())        
        {
          // ERROR
        }
    
        std::string response;
        auto it = mServices.find(json_get_string(v, "service"));
        if (it != mServices.end())
        {
            response = it->second->mHandler(v);
        }
        else
        {
            // ERROR
        }
        mServer.send(hdl, response, msg->get_opcode()); 
     }
    
    void WSServer::RunServer()
    {
        // Set logging settings
        mServer.set_access_channels(websocketpp::log::alevel::all);
        mServer.clear_access_channels(websocketpp::log::alevel::frame_payload);
        // Initialize Asio
        mServer.init_asio();
        // Register our message handler
        mServer.set_message_handler(bind(&WSServer::OnMessageReceived, this, _1, _2));
        // TODO: Port shouldn't be hardcoded
        mServer.listen(8080);
        // Start the server accept loop
        mServer.start_accept();
        // Start the ASIO io_service run loop
        mServer.run();
    }

    void WSServer::AddService(std::shared_ptr<WSService> service)
    {
        mServices[service->mServiceName] = service;
    }
    void WSServer::RemoveService(std::string serviceName)
    {
        mServices.erase(serviceName);
    }
}

