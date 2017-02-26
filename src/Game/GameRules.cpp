#include "Core/Server.h"
#include "Core/HttpClient.h"
#include "GameRules.h"
#include <iostream>
#include <fstream>
#include <string>


#define GAME_RULES_VERSION "POLYMINIS-V1"

namespace PolyminisGameRules
{
    float LERP(float x, float x0, float y0, float x1, float y1)
    {
        float a = (x - x0)/(x1 - x0);
        float b = 1 - a;
        return (y1*a + y0*b);
    }

    GameRules::GameRules(const std::string& filePath)
    {
        //
        LoadFromFile(filePath);
    }
    GameRules::GameRules(const picojson::value& config)
    {
        //
        LoadFromJsonObj(config);
    }

    GameRules::GameRules(const PolyminisServer::ServerCfg& cfg)
    {
        LoadFromDB(cfg);
    }

    float GameRules::GetMaxWarpDistance()
    {
        return MaxWarpDistance;
    }

    float GameRules::GetWarpCost(float eucDistance)
    {
        return BaseWarpCost + WarpCostMultiplier * EvalWarpCostCurve(eucDistance / GetMaxWarpDistance());
    }
    float GameRules::GetPercentageToBiomass(float percentage)
    {
        //TODO: Some math missing
        return EvalPercentageToBiomassCurve(percentage / 100.0);
    }
    float GameRules::GetBiomassToPercentage(float biomass)
    {
        //TODO: Some math missing
        return biomass;
    }

    void GenericSetCurve(std::vector<float> &curveValVec, const picojson::array& arr)
    {
        for(auto k : arr)
        {
            curveValVec.push_back(JsonHelpers::json_get_as_float(k));
        }
    }

    void GameRules::SetWarpCostCurve(const picojson::value& wCC)
    {
        if (!JsonHelpers::json_has_field(wCC, "WarpCostCurveKeyframes"))
            return;
        WarpCostCurveEvals.clear();
        auto curveSliceKeyframes = JsonHelpers::json_get_array(wCC, "WarpCostCurveKeyframes");
        GenericSetCurve(WarpCostCurveEvals, curveSliceKeyframes);
    }
    void GameRules::SetPercentageToBiomassCurve(const picojson::value& p2BC)
    {
        if (!JsonHelpers::json_has_field(p2BC, "PercentageToBiomassCurveKeyframes"))
            return;
        P2BCurveEvals.clear();
        auto curveSliceKeyframes = JsonHelpers::json_get_array(p2BC, "PercentageToBiomassCurveKeyframes");
        GenericSetCurve(P2BCurveEvals, curveSliceKeyframes);
    }
    void GameRules::SetBiomassToPercentageCurve(const picojson::value& b2PC)
    {
        if (!JsonHelpers::json_has_field(b2PC, "BiomassToPercentageCurveKeyframes"))
            return;
        B2PCurveEvals.clear();
        auto curveSliceKeyframes = JsonHelpers::json_get_array(b2PC, "BiomassToPercentageCurveKeyframes");
        GenericSetCurve(B2PCurveEvals, curveSliceKeyframes);
    }

    void GameRules::LoadFromFile(std::string path)
    {
        std::ifstream configFile;
        configFile.open(path);

        std::string line;
        std::string fileContent("");
        if (configFile.is_open())
        {
            while (getline(configFile,line))
            {
              fileContent += line;
            }
            configFile.close();
        }
        else
        {
            std::cout << "Error openning Config File: " << path << std::endl;
            return;
        }

        picojson::value loadedObj;
        std::cout << "Loaded From the Rules File: " << fileContent << std::endl;
        picojson::parse(loadedObj, fileContent);

        LoadFromJsonObj(loadedObj);
    }

    void GameRules::ReloadFromJson(const picojson::value& config)
    {
        LoadFromJsonObj(config);
    }
    void GameRules::LoadFromJsonObj(const picojson::value& config)
    {
        //TODO: This needs a lock right here <<->>
        if (JsonHelpers::json_has_field(config, "BaseWarpCost"))
            BaseWarpCost = JsonHelpers::json_get_float(config, "BaseWarpCost");
        if (JsonHelpers::json_has_field(config, "WarpCostMultiplier"))
            WarpCostMultiplier = JsonHelpers::json_get_float(config, "WarpCostMultiplier");
        if (JsonHelpers::json_has_field(config, "MaxWarpDistance"))
            MaxWarpDistance = JsonHelpers::json_get_float(config, "MaxWarpDistance");

        SetWarpCostCurve(config);
        SetPercentageToBiomassCurve(config);
        SetBiomassToPercentageCurve(config);

        // Save the serialized curves as Strings as we don't operate on them
        if (JsonHelpers::json_has_field(config, "WarpCostCurve"))
            WCAnimationCurveSerialized = picojson::value(JsonHelpers::json_get_object(config, "WarpCostCurve")).serialize();

        if (JsonHelpers::json_has_field(config, "BiomassToPercentageCostCurve"))
            B2PAnimationCurveSerialized = picojson::value(JsonHelpers::json_get_object(config, "BiomassToPercentageCostCurve")).serialize();

        if (JsonHelpers::json_has_field(config, "PercentageToBiomassCostCurve"))
            P2BAnimationCurveSerialized = picojson::value(JsonHelpers::json_get_object(config, "PercentageToBiomassCostCurve")).serialize();
    }

    void GameRules::ReloadFromDB(const PolyminisServer::ServerCfg& almanacServerCfg)
    {
        LoadFromDB(almanacServerCfg);
    }

    void GameRules::LoadFromDB(const PolyminisServer::ServerCfg& almanacServerCfg)
    {
        try
        {
            picojson::object db_resp = PolyminisServer::HttpClient::Request(almanacServerCfg.host,
                                                                            almanacServerCfg.port,
                                                                            "/persistence/gamerules/"+std::string(GAME_RULES_VERSION),
                                                                            PolyminisServer::HttpMethod::GET,
                                                                            picojson::object());

            auto db_resp_v = picojson::value(db_resp);
            auto rules = JsonHelpers::json_get_object(db_resp_v, "Response");
            LoadFromJsonObj(picojson::value(rules));
        }
        catch (websocketpp::exception const &e) 
        {
            std::cout << e.what() << std::endl;
        }
    }

    bool GameRules::SaveIntoFile(std::string filePath)
    {
        //
        std::ofstream configFile;
        configFile.open(filePath);
        auto config = SerializeRules();
        if (configFile.is_open())
        {
            configFile << config.serialize(); 
            configFile.close();
        }
        else
        {
            return false;
        }
        return true;
    }

    bool GameRules::SaveToDB(const PolyminisServer::ServerCfg& almanacServerCfg)
    {
        auto config = SerializeRules();

        PolyminisServer::HttpClient::Request(almanacServerCfg.host,
                                             almanacServerCfg.port,
                                             "/persistence/gamerules/"+std::string(GAME_RULES_VERSION),
                                             PolyminisServer::HttpMethod::POST,
                                             JsonHelpers::json_get_as_object(SerializeRules()));

        return true;
    }

    picojson::value GameRules::SerializeRules()
    {
        picojson::object config;

        auto keyframeSerializer =  [] (const std::vector<float> keyframes)
        {
            picojson::array returnable;
            for (float v : keyframes)
            {
                returnable.push_back(picojson::value(v));
            }
            return picojson::value(returnable);
        };

        config["WarpCostCurveKeyframes"] = keyframeSerializer(WarpCostCurveEvals);
        config["PercentageToBiomassCurveKeyframes"] = keyframeSerializer(P2BCurveEvals);
        config["BiomassToPercentageCurveKeyframes"] = keyframeSerializer(B2PCurveEvals);
        config["VERSION"] = picojson::value(GAME_RULES_VERSION);

        picojson::value curveObj;
        picojson::parse(curveObj, WCAnimationCurveSerialized);
        // An error here would be REALLY weird...

        config["WarpCostCurve"] = curveObj;
        config["BaseWarpCost"] = picojson::value(BaseWarpCost);
        config["WarpCostMultiplier"] = picojson::value(WarpCostMultiplier);
        config["MaxWarpDistance"] = picojson::value(MaxWarpDistance);

        return picojson::value(config);
    }

    float GenericEval(const std::vector<float>& slices, float t)
    {
        // Find the buckets that 'enclose' this value
        //
        float lowerBound = 0.0f;
        float upperBound = 1.0f;

        if (t < lowerBound)
        {
            return lowerBound;
        }

        if (t > upperBound)
        {
            return upperBound;
        }

        int slicesSize = slices.size();
        float dt = (1.0f / slicesSize);
        float t0 = 0.0f;
        float t1 = 1.0f; // Leave it at 1 in case slicesSize is 0 we can just LERP
        for(int i = 0; i < slicesSize; ++i)
        {
            t1 = t0 + dt; 
            if (t < t1)
            {
                // Found 
                float w = slices[i];
                lowerBound = w;
                if (i < slicesSize - 1)
                {
                    upperBound = slices[i + 1];
                }
                break;
            }
            t0 += dt; 
        }

        return LERP(t, t0, lowerBound, t1, upperBound);
    }

    float GameRules::EvalWarpCostCurve(float t)
    {
        return GenericEval(WarpCostCurveEvals, t);
    }

    float GameRules::EvalPercentageToBiomassCurve(float t)
    {
        return GenericEval(P2BCurveEvals, t);
    }

    float GameRules::EvalBiomassToPercentageCurve(float t)
    {
        return GenericEval(B2PCurveEvals, t);
    }
}
