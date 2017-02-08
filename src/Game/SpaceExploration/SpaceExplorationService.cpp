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
            for (auto a : all_planets)
            {

                float x   = 0.0f;
                float y   = 0.0f;
                int   pid = 1;
                if (payload.count("Position") > 0)
                {
                    auto position_json = payload["Position"];
                    x = JsonHelpers::json_get_float(position_json, "x");
                    y = JsonHelpers::json_get_float(position_json, "y");
                }

                if (payload.count("PlanetId") > 0)
                {
                    auto id_json = payload["PlanetId"];
                    id_json = JsonHelpers::json_get_float(position_json, "y");
                }


                std::cout << "  Adding Planet at: (" << x << "," << y << ")" << std::endl;
                mPlanetManager.AddPlanet(x, y, p_id);
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
        auto payload = JsonHelpers::json_get_object(request, "Payload");

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
