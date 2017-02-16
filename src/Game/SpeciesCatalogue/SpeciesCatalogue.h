#include <map>
#include <vector>
#include <string>

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
        std::vector<Splice> mSplices;
    };

    // keeps track of species the current user has
    class SpeciesCatalogueSession
    {
    public:
        explicit SpeciesCatalogueSession(const std::map<std::string, std::vector<Splice>> &inSpecies);
        SpeciesCatalogueSession() {}
        ~SpeciesCatalogueSession(){}

        // Grab the entire species dictionary from player, (verify it,) 
        // overwrite the map with it, and return true so stuff can
        // be pushed to almanac.
        bool AttemptSaveSpecies(const std::map<std::string, std::vector<Splice>> &inSpecies);

    private:
        int   mPlayerId { -1 };
        std::map<std::string, std::vector<Splice>> mSpecies;
    };
}
