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
        return biomass / 100.0;
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

    void GameRules::ReloadFromJson(const picojson::value& config)
    {
        LoadFromJsonObj(config);
    }
    void GameRules::LoadFromJsonObj(const picojson::value& config)
    {
        std::lock_guard<std::mutex>(*GameRulesLock);
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


        if (JsonHelpers::json_has_field(config, "SpliceData"))
        {
            SpliceData.clear();
            //
            auto splices = JsonHelpers::json_get_array(config, "SpliceData"); 
            
            for (auto s : splices) 
            {
                std::string s_key = JsonHelpers::json_get_string(s, "InternalName");
                SpliceData[s_key] = s;
            }
        }

        if (JsonHelpers::json_has_field(config, "TraitData"))
        {
            TraitData.clear();
            //
            auto traits = JsonHelpers::json_get_array(config, "TraitData"); 
            
            for (auto t : traits) 
            {
                std::string t_key = JsonHelpers::json_get_string(t, "InternalName");
                TraitData[t_key] = t;
            }
        }

        if (JsonHelpers::json_has_field(config, "DefaultTraits"))
        {
            DefaultTraits.clear();     
            auto dtraits = JsonHelpers::json_get_array(config, "DefaultTraits");

            for (auto dt : dtraits)
            {
                std::string dt_key = JsonHelpers::json_get_as_string(dt);

                DefaultTraits.push_back(TraitData[dt_key]);
            }
        }
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


    picojson::value GameRules::CreateInstinctWeights(const picojson::value& tuningConfig)
    {
        picojson::object jsonObj;

        // TODO: This needs some *serious* massaging:
        auto instinctKeys = {"Predatory", "Herding", "Hoarding", "Nomadic"};

        for (auto &i : instinctKeys)
        {
            float level = fmin(fmax(JsonHelpers::json_get_float(tuningConfig, std::string(i)+"Lvl"), 0), 8);

            float instinctWeight = 1 + (level / 10.0);

            jsonObj[i] = picojson::value(instinctWeight);
        }

        return picojson::value(jsonObj); 
    }

    picojson::value GameRules::CreateTranslationTable(const picojson::value& splices)
    {
        auto splices_arr = JsonHelpers::json_get_as_array(splices);

        picojson::array enabledTraits;
        for (auto s : splices_arr)
        {
            auto sName = JsonHelpers::json_get_string(s, "InternalName");
            auto sData = SpliceData[sName];

            auto traits = JsonHelpers::json_get_array(sData, "Traits");
            for (auto t : traits)
            {
                auto tName = JsonHelpers::json_get_as_string(t);
                auto trait = TraitData[tName];
                picojson::object trait_json; 
                trait_json["Number"] = picojson::value(JsonHelpers::json_get_float(trait, "TID"));
                trait_json["Tier"] = picojson::value(JsonHelpers::json_get_string(trait, "Tier"));
                enabledTraits.push_back(picojson::value(trait_json));
            }
        }

        for (auto trait : DefaultTraits)
        {
            picojson::object trait_json; 
            trait_json["Number"] = picojson::value(JsonHelpers::json_get_float(trait, "TID"));
            trait_json["Tier"] = picojson::value(JsonHelpers::json_get_string(trait, "Tier"));
            enabledTraits.push_back(picojson::value(trait_json));
        }

        std::cout << picojson::value(enabledTraits).serialize() << std::endl;
        return picojson::value(enabledTraits);
    }

    const picojson::object& GameRules::GetTraitData() const
    {
        return TraitData;
    }
}
