#include "Core/JsonHelpers.h"
#include "Core/HttpClient.h"
#include "GameUtils.h"

namespace GameDBUtils
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

    picojson::array GetAllSpeciesInPlanet(const PolyminisServer::ServerCfg& almanacServerCfg, const std::string& planetEpoch, bool summary)
    {
        std::string url = "/persistence/speciessummaries/";
        if (!summary)
        {
            url = "/persistence/speciesinplanet/";
        }
        picojson::object species_in_planet_resp = PolyminisServer::HttpClient::Request(almanacServerCfg.host,
                                                                                       almanacServerCfg.port,
                                                                                       url+planetEpoch,
                                                                                       PolyminisServer::HttpMethod::GET,
                                                                                       picojson::object());

        auto species_in_planet_resp_v = picojson::value(species_in_planet_resp);
        auto species_in_planet = JsonHelpers::json_get_object(species_in_planet_resp_v, "Response");
        auto species_in_planet_value = picojson::value(species_in_planet);
        return JsonHelpers::json_get_array(species_in_planet_value, "Items");
    }

    static picojson::object GetSpeciesInPlanet_internal(const PolyminisServer::ServerCfg& almanacServerCfg, const std::string& planetEpoch, const std::string& speciesName,
                                                        bool summary = true)
    {
        std::string url = "/persistence/speciessummaries/";
        if (!summary)
        {
            url = "/persistence/speciesinplanet/";
        }

        picojson::object species_resp = PolyminisServer::HttpClient::Request(almanacServerCfg.host,
                                                                             almanacServerCfg.port,
                                                                             url+planetEpoch+"/"+speciesName,
                                                                             PolyminisServer::HttpMethod::GET,
                                                                             picojson::object());

        auto species_resp_v = picojson::value(species_resp);
        auto species = JsonHelpers::json_get_object(species_resp_v, "Response");
        return species;
    }

    picojson::object GetSpeciesInPlanet(const PolyminisServer::ServerCfg& almanacServerCfg, const std::string& planetEpoch, const std::string& speciesName)
    {
        return GetSpeciesInPlanet_internal(almanacServerCfg, planetEpoch, speciesName, true);
    }

    picojson::object GetSpeciesGeneticPayload(const PolyminisServer::ServerCfg& almanacServerCfg, const std::string& planetEpoch, const std::string& speciesName)
    {
        return GetSpeciesInPlanet_internal(almanacServerCfg, planetEpoch, speciesName, false);
    }
}

namespace GameSimUtils
{
    picojson::object CreateSimulationServer(const PolyminisServer::ServerCfg& simServerCfg)
    {
        picojson::object createResp = PolyminisServer::HttpClient::Request(simServerCfg.host,
                                                                           simServerCfg.port,
                                                                           "/simulations/add",
                                                                           PolyminisServer::HttpMethod::POST,
                                                                           picojson::object());
    
        auto createResp_v = picojson::value(createResp);
        auto create = JsonHelpers::json_get_object(createResp_v, "Response");
        return create;
    }
    picojson::array GetSimulationSteps(const PolyminisServer::ServerCfg& simServerCfg, int simSessionId, int& inout_epoch, int& inout_startAtStep, int maxSteps)
    {
        picojson::array steps;
        int latest_step = 0;

        // Check in /epochs


        auto epoch_url = "/simulations/"+std::to_string(simSessionId)+"/epochs/"+std::to_string(inout_epoch);

        picojson::object epochsResp = PolyminisServer::HttpClient::Request(simServerCfg.host,
                                                                           simServerCfg.port,
                                                                           epoch_url,
                                                                           PolyminisServer::HttpMethod::GET,
                                                                           picojson::object());

        auto epochsResp_v = picojson::value(epochsResp);
        auto epochs = JsonHelpers::json_get_object(epochsResp_v, "Response");

        latest_step = std::min((int)JsonHelpers::json_get_as_float(epochs["Steps"]), (inout_startAtStep + maxSteps));
        
        while(inout_startAtStep < latest_step)
        {
            picojson::object stepResp = PolyminisServer::HttpClient::Request(simServerCfg.host,
                                                                             simServerCfg.port,
                                                                             epoch_url+"/steps/"+std::to_string(inout_startAtStep)+"/",
                                                                             PolyminisServer::HttpMethod::GET,
                                                                             picojson::object());
            auto stepResp_v = picojson::value(stepResp);
            auto step = JsonHelpers::json_get_object(stepResp_v, "Response");

            steps.push_back(picojson::value(step));

            inout_startAtStep++;
        }

        return steps;
    }


    picojson::array GetSpecies(const PolyminisServer::ServerCfg& simServerCfg, int simSessionId, int epoch)
    {
        auto url = "/simulations/"+std::to_string(simSessionId)+"/epochs/"+std::to_string(epoch)+"/species";
        picojson::object speciesResp = PolyminisServer::HttpClient::Request(simServerCfg.host,
                                                                            simServerCfg.port,
                                                                            url,
                                                                            PolyminisServer::HttpMethod::GET,
                                                                            picojson::object());
        auto speciesResp_v = picojson::value(speciesResp);  
        auto species = JsonHelpers::json_get_object(speciesResp_v, "Response");  
        return JsonHelpers::json_get_array(picojson::value(species), "Species");

    }
    picojson::object GetEnvironment(const PolyminisServer::ServerCfg& simServerCfg, int simSessionId, int epoch)
    {
        auto url = "/simulations/"+std::to_string(simSessionId)+"/epochs/"+std::to_string(epoch)+"/environment";
        picojson::object envResp = PolyminisServer::HttpClient::Request(simServerCfg.host,
                                                                        simServerCfg.port,
                                                                        url,
                                                                        PolyminisServer::HttpMethod::GET,
                                                                        picojson::object());
        auto envResp_v = picojson::value(envResp);  
        auto env = JsonHelpers::json_get_object(envResp_v, "Response");  
        return JsonHelpers::json_get_object(picojson::value(env), "Environment");
    }

    picojson::object RunSimulation(const PolyminisServer::ServerCfg& simServerCfg, const PolyminisServer::ServerCfg& almanacServerCfg, const picojson::object& masterTT,
                                   int simSessionId, int planetId, int epoch)
    {
        // Get the Planet-Epoch data from the DB
        auto species = GameDBUtils::GetAllSpeciesInPlanet(almanacServerCfg, std::to_string(planetId)+std::to_string(epoch), false);

        // TODO: Configuration is hardcoded currently - If anything changes here, it should be changed in Chronos as well!!
        
        picojson::object simData;
        picojson::object envObj;
        picojson::object epochObj;
        picojson::array sensorList;

        auto v = {"positionx","positiony","orientation","lastmovesucceded"};
        for (auto s : v)
        {
            sensorList.push_back(picojson::value(s));
        }
        
        envObj["DefaultSensors"] = picojson::value(sensorList);

        picojson::object dimObj;
        dimObj["x"] = picojson::value(100.0);
        dimObj["y"] = picojson::value(100.0);

        envObj["Dimensions"] = picojson::value(dimObj);
        envObj["SpeciesSlots"] = picojson::value((double)3);

        epochObj["Environment"] = picojson::value(envObj);
        epochObj["MaxSteps"] = picojson::value((double)50);
        epochObj["Restarts"] = picojson::value((double)0);
        epochObj["Substeps"] = picojson::value((double)4);
        epochObj["Proportions"] = picojson::value(picojson::array());

        // Send the simulation configuration to the server
        simData["EpochNum"] = picojson::value((double)epoch);
        simData["Epoch"] = picojson::value(epochObj);


        picojson::array flatMasterTT;
        for(auto kv : masterTT)
        {
            flatMasterTT.push_back(kv.second);
        }

        simData["MasterTranslationTable"] = picojson::value(flatMasterTT);

        std::cout << "SIMULATION DATA" << std::endl;
        std::cout << picojson::value(simData).serialize() << std::endl;


        simData["Species"] = picojson::value(species);
        picojson::object loadResp = PolyminisServer::HttpClient::Request(simServerCfg.host,
                                                                         simServerCfg.port,
                                                                         "/simulations/"+std::to_string(simSessionId)+"/epochs/simulate",
                                                                         PolyminisServer::HttpMethod::POST,
                                                                         simData);
        auto loadResp_v = picojson::value(loadResp);
        auto load = JsonHelpers::json_get_object(loadResp_v, "Response");
        return load;

    }
}
