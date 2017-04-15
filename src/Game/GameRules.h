#pragma once
#include "Core/JsonHelpers.h"

#include <mutex>
#include <vector>

namespace PolyminisGameRules
{
    class GameRules
    {
    public:
        GameRules(const std::string& filePath);
        GameRules(const picojson::value& config);
        GameRules(const PolyminisServer::ServerCfg& almanacServerCfg);

        float GetWarpCost(float eucDistance);
        float GetPercentageToBiomass(float percentage); 
        float GetBiomassToPercentage(float biomass); 
        float GetMaxWarpDistance();

        const picojson::object& GetSpliceData() const;
        const picojson::object& GetTraitData() const;

        bool SaveToDB(const PolyminisServer::ServerCfg& almanacServerCfg);

        void ReloadFromJson(const picojson::value& config);
        void ReloadFromDB(const PolyminisServer::ServerCfg& almanacServerCfg);
        picojson::value SerializeRules();

        picojson::value CreateTranslationTable(const picojson::value& splices);
        picojson::value CreateInstinctWeights(const picojson::value& splices);

    private:
        void SetWarpCostCurve(const picojson::value& wCC);
        void SetPercentageToBiomassCurve(const picojson::value& p2BC);
        void SetBiomassToPercentageCurve(const picojson::value& b2PC);

        void LoadFromDB(const PolyminisServer::ServerCfg& almanacServerCfg);
        void LoadFromJsonObj(const picojson::value& config);

        float EvalWarpCostCurve(float t);
        float EvalPercentageToBiomassCurve(float t);
        float EvalBiomassToPercentageCurve(float t);

// Members
//
//
        
//
        std::shared_ptr<std::mutex> GameRulesLock;

// Rules
        std::vector<float> WarpCostCurveEvals;
        std::string WCAnimationCurveSerialized;

        // X-percentage extracted from a Planet -> Y Amount of Biommas
        std::vector<float> P2BCurveEvals;
        std::string P2BAnimationCurveSerialized;

        // X-Biomass Deployed into a Planet -> Y Percentage of Population
        std::vector<float> B2PCurveEvals;
        std::string B2PAnimationCurveSerialized;        

        float BaseWarpCost;
        float WarpCostMultiplier;
        float MaxWarpDistance;

// Static Data
        std::unordered_map<std::string, picojson::value> SpliceData;
        // Trait Data is a table from internal name to TierId and Number
        picojson::object TraitData;
        
        std::vector<picojson::value> DefaultTraits; 

    };
}
