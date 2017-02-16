#include "SpeciesCatalogue.h"

namespace SpeciesCatalogue
{
    SpeciesCatalogueSession::SpeciesCatalogueSession(const std::map<std::string, std::vector<Splice>> &inSpecies) :
        mSpecies(inSpecies)
    {

    }

    bool SpeciesCatalogueSession::AttemptSaveSpecies(const std::map<std::string, std::vector<Splice>> &inSpecies)
    {
        mSpecies = inSpecies;
        return true;
    }
}