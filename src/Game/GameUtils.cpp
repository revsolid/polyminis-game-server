#include "Core/HttpClient.h"
#include "GameUtils.h"

namespace GameUtils
{
    bool SaveBiomassValue(const PolyminisServer::SessionData& session, const PolyminisServer::ServerCfg& almanacServerCfg)
    {
        
        try
        {
            picojson::object bmPayload;
            bmPayload["Biomass"] = picojson::value(session.BiomassAvailable);
            PolyminisServer::HttpClient::Request(almanacServerCfg.host, almanacServerCfg.port,
                                                "/persistence/users"+session.UserName+"/",
                                                 PolyminisServer::HttpMethod::PUT, bmPayload);
            return true;

        }
        catch (websocketpp::exception const & e)
        {
            std::cout << e.what() << std::endl;
            return false;
        }
    }
}
