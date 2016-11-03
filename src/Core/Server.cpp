#include "Server.h"

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
            std::cout << "Error Parsing Message: " << message << std::endl;
        }
    
        picojson::object response;
        auto it = mServices.find(JsonHelpers::json_get_string(v, "Service"));
        if (it != mServices.end())
        {
            std::cout << "Service Found: " << message << std::endl;
            response = it->second->mHandler(v);
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

    namespace JsonHelpers
    {
        // TODO: I think this should be done with templates, but those things scare the shit out of me
        std::string json_get_string(picojson::value& v, const std::string& fieldname)
        {
            //check if the type of the value is "object"
            if (! v.is<picojson::object>())
            {
                // ERROR
                std::cout << "Error value is not an object (json_get_string)" << std::endl;
            }
            // obtain a const reference to the map
            const picojson::value::object& obj = v.get<picojson::object>();
            auto it = obj.find(fieldname);
            if (it == obj.end())
            {
                // ERROR 
                std::cout << "Error field: "<< fieldname << " not found" << std::endl;
                return "";
            }
            
            // Use to_str to give ownership to the caller
            return it->second.to_str();
        }

        float json_get_float(picojson::value& v, const std::string& fieldname)
        {
            //check if the type of the value is "object"
            if (! v.is<picojson::object>())
            {
                // ERROR
                std::cout << "Error value is not an object (json_get_float)" << std::endl;
            }
            // obtain a const reference to the map
            const picojson::value::object& obj = v.get<picojson::object>();
            auto it = obj.find(fieldname);
            if (it == obj.end())
            {
               // ERROR 
               std::cout << "Error field: "<< fieldname << " not found" << std::endl;
               return 0.0f;
            }
            return (float) it->second.get<double>();
        }


        const picojson::object& json_get_object(picojson::value& v, const std::string& fieldname)
        {
            if (! v.is<picojson::object>())
            {
                // ERROR
            }
            return v.get<picojson::object>();
        }

        picojson::object json_create_error(const std::string& e_msg)
        {
            
            std::cout << "Creating Error: " << e_msg << std::endl;
            picojson::object obj;
            obj["error"] = picojson::value(e_msg);
            return  obj;
        }
    }
}
