#include "SpeciesCatalogue.h"
#include "Core/JsonHelpers.h"

namespace SpeciesCatalogue
{
    SpeciesCatalogueSession::SpeciesCatalogueSession(const std::vector<Species> &inSpecies) :
        mSpecies(inSpecies)
    {

    }

    bool SpeciesCatalogueSession::AttemptSaveSpecies(picojson::array &speciesArray)
    {
        for (picojson::array::iterator iter = speciesArray.begin(); iter != speciesArray.end(); ++iter) 
        {
            try
            {
                //JsonHelpers::json_get_array((*iter), "Splices");
                Species toAdd;
                toAdd.mName = (*iter).get("Name").get<std::string>();
                std::cout << "Species received from client [" << toAdd.mName << "]" << std::endl;

                // splices
                picojson::array spliceArray = (*iter).get("Splices").get<picojson::array>();
                for (picojson::array::iterator spliceIter = spliceArray.begin(); spliceIter != spliceArray.end(); ++spliceIter)
                {
                    std::string receivedInternalName = JsonHelpers::json_get_string((*spliceIter), "InternalName");
                    std::cout << toAdd.mName << ": Splices[" << receivedInternalName << "]" << std::endl;

                    std::shared_ptr<Splice> pSplice;
                    // now, check the splice list to see if there the splice is in there.
                    pSplice = getSpliceFromList(receivedInternalName);
                    if(pSplice == nullptr)
                    {
                        std::cout << "SpeciesCatalogueSession: Error: Splice name not found." << std::endl;
                        return false;
                    }

                    // it is in the splice list then...now add it
                    toAdd.AddSplice(pSplice);
                }

                std::shared_ptr<Species> pSpecies = getSpeciesFromList(toAdd.mName);
                if(pSpecies != nullptr)
                {
                    // already in there. need to override it. 
                    pSpecies->mSplices = toAdd.mSplices;
                }
                else
                {
                    mSpecies.push_back(toAdd);
                } 

            }
            catch (const std::exception &e)
            {
                std::cout << "Exception: converting from json to species." << std::endl;
                return false;
            }
        }
        // TODO: delete species Species list that are gone now.

        return true;
    }

    std::shared_ptr<Species> SpeciesCatalogueSession::getSpeciesFromList(std::string &name)
    {
        for(Species s : mSpecies)
        {
            if (s.mName == name)
            {
                return std::make_shared<Species>(s);
            }
        }
        return nullptr;
    }

    std::shared_ptr<Splice> SpeciesCatalogueSession::getSpliceFromList(std::string &internalName)
    {
        for(Splice s : mSplices)
        {
            if (s.mInternalName == internalName)
            {
                return std::make_shared<Splice>(s);
            }
        }
        return nullptr;
    }

}