#include <vector>
#include <string>
#include <memory>
#include "Core/JsonHelpers.h"

namespace SpeciesCatalogue
{
    struct Splice
    {
        std::string mInstinct;
        std::string mName;
        std::string mInternalName;
        std::string mDescription;
        std::vector<int> mTraits;
    };

    struct Species
    {
        std::string mName;
        std::vector<std::shared_ptr<Splice>> mSplices;

        // get slice inside the species, if not found, return null.
        std::shared_ptr<Splice> GetSplice(const std::string &internalName);

        // return true if did add, false if it's already there so no add.
        bool AddSplice(std::shared_ptr<Splice> pSplice);

        // return the species as a json object, with species name and 
        // internal names for each splice.
        picojson::object GetJsonObject();
    };

    // keeps track of species the current user has
    // this is for user inventory
    class SpeciesCatalogueSession
    {
    public:
        explicit SpeciesCatalogueSession(const std::vector<Species> &inSpecies);
        SpeciesCatalogueSession() {}
        ~SpeciesCatalogueSession(){}

        // Grab the entire species dictionary from player, (verify it,) 
        // overwrite the map with it, and return true so stuff can
        // be pushed to almanac.
        bool AttemptSaveSpecies( picojson::array &speciesArray );
        
        bool AddSplice(Splice &inSplice);
    
        std::vector<Species> GetSpecies();

        picojson::array GetSpeciesJsonArray();


    private:
        int   mPlayerId { -1 };
        std::vector<Species> mSpecies;
        std::vector<Splice> mSplices; // filled from Almanac
        
        std::shared_ptr<Species> getSpeciesFromList(std::string &name);
        std::shared_ptr<Splice> getSpliceFromList(std::string &internalName);
    };
}
