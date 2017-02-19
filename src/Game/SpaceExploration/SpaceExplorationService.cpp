#include "SpaceExplorationService.h"
#include "Core/JsonHelpers.h"
#include "Core/HttpClient.h"
#include "Game/GameUtils.h"

namespace SpaceExploration
{
    SpaceExplorationService::SpaceExplorationService(PolyminisServer::WSServer& server,
                                                     PlanetManager& pManager,
                                                     PolyminisServer::ServerCfg almanacServerCfg) :
                                                     mPlanetManager(pManager),
                                                     mAlmanacServerCfg(almanacServerCfg)
    {
        auto wss = std::make_shared<PolyminisServer::WSService>();
        wss->mServiceName = "space_exploration";
        wss->mHandler =  [=] (picojson::value& request, PolyminisServer::SessionData& sd)
                         {
                             return this->SpaceExplorationEndpoint(request, sd);
                         };
        server.AddService(wss);

        try
        {
            std::cout << "Requesting Almanac Server for Persistent Planets..." << std::endl;
            picojson::object planets_resp = PolyminisServer::HttpClient::Request(mAlmanacServerCfg.host,
                                                                                 mAlmanacServerCfg.port,
                                                                                 "/persistence/planets/",
                                                                                 PolyminisServer::HttpMethod::GET,
                                                                                 picojson::object());

            auto planets_resp_v = picojson::value(planets_resp);
            auto planets = JsonHelpers::json_get_object(planets_resp_v, "Response");
            auto planets_value = picojson::value(planets);
            picojson::array all_planets =  JsonHelpers::json_get_array(planets_value, "Items");

            std::cout << "Adding planets to Planet Manager..." << std::endl;
            for (auto planet : all_planets)
            {
                float x   = 0.0f;
                float y   = 0.0f;
                int   pid = mPlanetManager.GetNextPlanetId();
                int   epoch = 0;

                float t_min = 0.0f;
                float t_max = 0.0f;
                float ph_min = 0.0f;
                float ph_max = 0.0f;

                std::string name = "Test Planet";
                if (JsonHelpers::json_has_field(planet, "SpacePosition"))
                {
                    auto position_json = picojson::value(JsonHelpers::json_get_object(planet, "SpacePosition"));
                    x = JsonHelpers::json_get_float(position_json, "x");
                    y = JsonHelpers::json_get_float(position_json, "y");
                }

                if (JsonHelpers::json_has_field(planet, "PlanetId"))
                {
                    pid = JsonHelpers::json_get_int(planet, "PlanetId");
                }

                
                if (JsonHelpers::json_has_field(planet, "Temperature"))
                {
                    auto temp_json = picojson::value(JsonHelpers::json_get_object(planet, "Temperature"));
                    t_min = JsonHelpers::json_get_float(temp_json, "Min");
                    t_max = JsonHelpers::json_get_float(temp_json, "Max");
                }

                if (JsonHelpers::json_has_field(planet, "Ph"))
                {
                    auto ph_json = picojson::value(JsonHelpers::json_get_object(planet, "Ph"));
                    ph_min = JsonHelpers::json_get_float(ph_json, "Min");
                    ph_max = JsonHelpers::json_get_float(ph_json, "Max");
                }

                if (JsonHelpers::json_has_field(planet, "Epoch"))
                {
                    epoch = JsonHelpers::json_get_int(planet, "Epoch");
                }

                if (JsonHelpers::json_has_field(planet, "PlanetName"))
                {
                    name = JsonHelpers::json_get_string(planet, "PlanetName");
                }

                picojson::object species_in_planet_resp = PolyminisServer::HttpClient::Request(mAlmanacServerCfg.host,
                                                                                               mAlmanacServerCfg.port,
                                                                                               "/persistence/speciessummaries/"+std::to_string(pid)+std::to_string(epoch),
                                                                                               PolyminisServer::HttpMethod::GET,
                                                                                               picojson::object());

                auto species_in_planet_resp_v = picojson::value(species_in_planet_resp);
                auto species_in_planet = JsonHelpers::json_get_object(species_in_planet_resp_v, "Response");
                auto species_in_planet_value = picojson::value(species_in_planet);
                picojson::array all_species_in_planet =  JsonHelpers::json_get_array(species_in_planet_value, "Items");

                std::cout << "  Adding Planet " << name << " with: " << pid << " at: (" << x << "," << y << ")" << std::endl;
                std::cout << "    Temperature [" << t_min << "," << t_max << "]" << std::endl;
                std::cout << "    Ph          [" << ph_min << "," << ph_max << "]" << std::endl;
                std::cout << "    With "<< all_species_in_planet.size() << " Species: " << std::endl;

                Planet planetModel(x, y, pid, t_min, t_max, ph_min, ph_max, name);
                for (auto summary : all_species_in_planet)
                {
                    SpeciesSummary ss = SpeciesSummary::FromJson(summary);
                    std::cout << "      Species: " << ss.Name << " [" << ss.Percentage << "]" << std::endl;
                    planetModel.AddSpecies(std::move(ss));
                } 
                mPlanetManager.AddPlanet(planetModel);
            }
        }
        catch (websocketpp::exception const & e)
        {
            std::cout << e.what() << std::endl;
        }
    }

    picojson::object SpaceExplorationService::SpaceExplorationEndpoint(picojson::value& request,
                                                                       PolyminisServer::SessionData& sd)
    {
        std::string command = JsonHelpers::json_get_string(request, "Command");
        auto payload = JsonHelpers::json_get_as_object(request);

        if (payload.count("Position") == 0)
        {
            return JsonHelpers::json_create_error("Error - Position not sent");
        }
        auto position_json = payload["Position"];
        float x = JsonHelpers::json_get_float(position_json, "x");
        float y = JsonHelpers::json_get_float(position_json, "y");

        picojson::object to_ret;
        to_ret["Service"] = picojson::value("space_exploration");
        if (command == "INIT")
        {
            // Return ID?
            to_ret = picojson::object();
        }
        else if (command == "ATTEMPT_MOVE")
        {
            if (false)
            {
                // TODO: kickback
            }
            else
            {
                CreatePlanetaryPayload(to_ret, sd.Position, sd.VisibilityRange);
                sd.Position = Coord { x, y };
            }
        }
        else if (command == "WARP")
        {
            Coord destPoint {x, y};
            //destPoint.x = x;
            //destPoint.y = y;
            float warpCost = CalcWarpCost(sd.Position, destPoint);
            bool canWarp = (sd.BiomassAvailable > warpCost); 

            to_ret["EventString"] = picojson::value("WARP");
            if (canWarp)
            {
                CreateWarpPayload(to_ret, destPoint);
                sd.Position = destPoint;
                to_ret["PositionSaved"] = picojson::value(SavePositionOnDB(destPoint, sd.UserName));
                sd.BiomassAvailable -= warpCost;
                to_ret["NewBiomassAvailable"] = picojson::value(sd.BiomassAvailable); 

                GameUtils::SaveBiomassValue(sd, mAlmanacServerCfg);
            }
            else
            {
                to_ret["ErrorMessage"] = picojson::value("NOT_ENOUGH_BIOMASS");
            }
        }
        else if (command == "CALC_WARP_COST")
        {
            Coord destPoint {x, y};
            float warpCost = CalcWarpCost(sd.Position, destPoint); // What's the warp cost?
            to_ret["WarpCost"] = picojson::value(warpCost);
            to_ret["EventString"] = picojson::value("WARP_COST_RESULT");
        }
        else if (command == "SAVE_POSITION")
        {
            to_ret["PositionSaved"] = picojson::value(SavePositionOnDB(Coord {x, y}, sd.UserName));
        }
        else
        {
            to_ret = JsonHelpers::json_create_error("Command Not Found");
        }

    
        return std::move(to_ret);
    }

    void SpaceExplorationService::CreatePlanetaryPayload(picojson::object& planetsSpawnEvent, const Coord& pos, float visRange)
    {

        std::cout << "Creating Planetary Payload " << std::endl;
        picojson::array visiblePlanets = mPlanetManager.GetVisiblePlanets(pos, visRange);
        if (!visiblePlanets.empty())
        {
            planetsSpawnEvent["EventString"] = picojson::value("SPAWN_PLANETS");
            planetsSpawnEvent["Planets"] = picojson::value(visiblePlanets);
        }
    }

    void SpaceExplorationService::CreateWarpPayload(picojson::object& warpToPlanetEvent, const Coord& dest)
    {
        std::cout << "Creating Warp Payload " << std::endl;

        picojson::object position_json;
        position_json["x"] = picojson::value(dest.x);
        position_json["y"] = picojson::value(dest.y);
        warpToPlanetEvent["Position"] = picojson::value(position_json);
    }

    bool SpaceExplorationService::SavePositionOnDB(const Coord& inPos, const std::string& userName)
    {
        try
        {
            picojson::object lkpPayload;
            picojson::object pos;
            pos["x"] = picojson::value(inPos.x);
            pos["y"] = picojson::value(inPos.y);
            lkpPayload["LastKnownPosition"] = picojson::value(pos);
            PolyminisServer::HttpClient::Request(mAlmanacServerCfg.host, mAlmanacServerCfg.port, "/persistence/users/"+userName+"/",
                                                 PolyminisServer::HttpMethod::PUT, lkpPayload);
        }
        catch (websocketpp::exception const & e)
        {
            std::cout << e.what() << std::endl;
            return false;
        }
        return true;
    }

    float SpaceExplorationService::CalcWarpCost(const Coord& start, const Coord& target)
    {
        // TODO: What are the rules? Can we make this data-driven or something that doesn't require recompiling
        // to change :) ?
        return 1.0f;
    }
}
