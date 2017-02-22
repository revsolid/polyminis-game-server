#include "Core/JsonHelpers.h"
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
                                                "/persistence/users/"+session.UserName,
                                                 PolyminisServer::HttpMethod::PUT, bmPayload);
            return true;

        }
        catch (websocketpp::exception const & e)
        {
            std::cout << e.what() << std::endl;
            return false;
        }
    }

    picojson::array GetSpeciesInPlanet(const PolyminisServer::ServerCfg& almanacServerCfg, const std::string& planetEpoch)
    {
        std::cout << "XXXXXXXX" << std::endl;
        picojson::object species_in_planet_resp = PolyminisServer::HttpClient::Request(almanacServerCfg.host,
                                                                                       almanacServerCfg.port,
                                                                                       "/persistence/speciessummaries/"+planetEpoch,
                                                                                       PolyminisServer::HttpMethod::GET,
                                                                                       picojson::object());

        auto species_in_planet_resp_v = picojson::value(species_in_planet_resp);
        auto species_in_planet = JsonHelpers::json_get_object(species_in_planet_resp_v, "Response");
        auto species_in_planet_value = picojson::value(species_in_planet);
        return JsonHelpers::json_get_array(species_in_planet_value, "Items");
    }
}
