#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include "Types.h"

class Tools
{
public:
	// parse a commma seperated string into a string vector
	static std::vector<std::string> ParseCommaString(std::string inString)
	{
		std::vector<std::string> retVal;
		std::stringstream ss(inString);
		std::string token;

		while (std::getline(ss, token, ',')) 
		{
			retVal.push_back(token);
		}

		ss.str("");

		return retVal;
	}

	static Coord Parse2DCoord(std::string inString)
	{
		std::vector<std::string> svector = ParseCommaString(inString);
		Coord retVal;

		retVal.x = atof(svector[0].c_str());
		retVal.y = atof(svector[1].c_str());

		return retVal;
	}
};
