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

    bool SpeciesCatalogueSession::AddSplice(Splice &inSplice)
    {
        if(getSpliceFromList(inSplice.mInternalName) == nullptr)
        {
           mSplices.push_back(inSplice);
           return true;
        }
        return false;
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

    picojson::array SpeciesCatalogueSession::GetSpeciesJsonArray()
    {
        picojson::array retVal;

        for (auto s : mSpecies)
        {
            retVal.push_back(picojson::value(s.GetJsonObject()));
        }

        return retVal;
    }

    std::vector<Species> SpeciesCatalogueSession::GetSpecies()
    {
        return mSpecies;
    }


    // get slice inside the species, if not found, return null
    std::shared_ptr<Splice> Species::GetSplice(const std::string &internalName)
    {
        for(std::shared_ptr<Splice> s : mSplices)
        {
            if(s->mInternalName == internalName)
            {
                return s;
            }
        }
        return nullptr;
    }

    // return true if did add, false if it's already there so no add
    bool Species::AddSplice(std::shared_ptr<Splice> pSplice)
    {
        for(std::shared_ptr<Splice> s : mSplices)
        {
            if(s == pSplice)
            {
                return false;
            }
        }
        mSplices.push_back(pSplice);
        return true;
    }

    // return the species as a json object, with species name and 
    // internal names for each splice.
    picojson::object Species::GetJsonObject()
    {
        picojson::object obj;
        picojson::array spliceArray;

        obj["Name"] = picojson::value(mName);
        for(auto splice : mSplices)
        {
            picojson::object spliceObj;
            spliceObj["InternalName"] = picojson::value(splice->mInternalName);
            spliceArray.push_back(picojson::value(spliceObj));
        }
        obj["Splices"] = picojson::value(spliceArray);

        return obj;
    }

}


