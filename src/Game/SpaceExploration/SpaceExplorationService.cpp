#include "SpaceExplorationService.h"

namespace SpaceExploration
{
    SpaceExplorationService::SpaceExplorationService(PolyminisServer::WSServer& server,
                                                     PlanetManager& pManager) :
                                                     mPlanetManager(pManager)
    {
        auto wss = std::make_shared<PolyminisServer::WSService>();
        wss->mServiceName = "space_exploration";
        wss->mHandler =  [=] (picojson::value& request)
                         {
                             return this->SpaceExplorationEndpoint(request);
                         };
        server.AddService(wss);
    }
    
    picojson::object SpaceExplorationService::SpaceExplorationEndpoint(picojson::value& request)
    {
        std::string command = PolyminisServer::JsonHelpers::json_get_string(request, "Command");
        auto payload = PolyminisServer::JsonHelpers::json_get_object(request, "Payload");
        if (payload.count("Position") == 0)
        {
            return PolyminisServer::JsonHelpers::json_create_error("Error - Position not sent");
        }
        auto position_json = payload["Position"];
        float x = PolyminisServer::JsonHelpers::json_get_float(position_json, "x");
        float y = PolyminisServer::JsonHelpers::json_get_float(position_json, "y");
       
    
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
        else
        {
            to_ret = PolyminisServer::JsonHelpers::json_create_error("Command Not Found");
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
}
