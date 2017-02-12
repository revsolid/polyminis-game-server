#include "SpaceExplorationService.h"
#include "Core/JsonHelpers.h"
#include "Core/HttpClient.h"

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
        wss->mHandler =  [=] (picojson::value& request)
                         {
                             return this->SpaceExplorationEndpoint(request);
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
                int   pid = 1;

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

                if (JsonHelpers::json_has_field(planet, "PlanetName"))
                {
                    name = JsonHelpers::json_get_string(planet, "PlanetName");
                }

                std::cout << "  Adding Planet " << name << " with: " << pid << " at: (" << x << "," << y << ")" << std::endl;
                std::cout << "    Temperature [" << t_min << "," << t_max << "]" << std::endl;
                std::cout << "    Ph          [" << ph_min << "," << ph_max << "]" << std::endl;
                mPlanetManager.AddPlanet(Planet(x, y, pid, t_min, t_max, ph_min, ph_max, name));
            }
	}
        catch (websocketpp::exception const & e)
        {
            std::cout << e.what() << std::endl;
        }
    }

    picojson::object SpaceExplorationService::SpaceExplorationEndpoint(picojson::value& request)
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
        if (command == "INIT")
        {
            mSpaceMapSession = SpaceMapSession(Coord {x, y});
            // Return ID?
            to_ret = picojson::object();
        }
        else if (command == "ATTEMPT_MOVE")
        {
            if (!mSpaceMapSession.AttemptMove(Coord {x, y}))
            {
                // TODO: kickback
            }
            else
            {
                to_ret = CreatePlanetaryPayload();
            }
        }
        else if (command == "WARP")
        {
            Coord destPoint;
            destPoint.x = x;
            destPoint.y = y;
            if (mSpaceMapSession.AttemptWarp(mPlanetManager, destPoint))
            {
                to_ret = CreateWarpPayload(destPoint);
            }
        }
        else
        {
            to_ret = JsonHelpers::json_create_error("Command Not Found");
        }
        return std::move(to_ret);
    }

    picojson::object SpaceExplorationService::CreatePlanetaryPayload()
    {

        std::cout << "Creating Planetary Payload " << std::endl;
        picojson::array visiblePlanets = mPlanetManager.GetVisiblePlanets(mSpaceMapSession.GetPos(), mSpaceMapSession.GetVisibilityRange());
        if (!visiblePlanets.empty())
        {
            picojson::object planetsSpawnEvent;
            planetsSpawnEvent["Service"] = picojson::value("space_exploration");
            planetsSpawnEvent["EventString"] = picojson::value("SPAWN_PLANETS");
            planetsSpawnEvent["Planets"] = picojson::value(visiblePlanets);
            return planetsSpawnEvent;
        }
        return std::move(picojson::object());
    }

    picojson::object SpaceExplorationService::CreateWarpPayload(Coord dest)
    {
        std::cout << "Creating Warp Payload " << std::endl;
        picojson::object warpToPlanetEvent;
        warpToPlanetEvent["Service"] = picojson::value("space_exploration");
        warpToPlanetEvent["EventString"] = picojson::value("WARP");

        picojson::object position_json;
        position_json["x"] = picojson::value(dest.x);
        position_json["y"] = picojson::value(dest.y);
        warpToPlanetEvent["Position"] = picojson::value(position_json);
        return warpToPlanetEvent;
    }

}
