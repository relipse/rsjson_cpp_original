/**
 *  Really Simple Json Parser/Loader/Saver
 *  This uses a recursive descent parser to parse/save/load json objects.
 *
 *
 */
#ifndef RSJSON_H
#define RSJSON_H

#include <vector>
#include <string>
#include <map>

#include <stdexcept>


using namespace std;


#ifdef DEBUG_RSJSON
#define DEBUG_PRINTF printf
#else //if we are not debugging, make all the printf statements do nothing
#define DEBUG_PRINTF(...) do{} while(0)
#endif


enum rsjquotestyletype { qsBEST, qsSINGLE, qsDOUBLE };

class RSJsonUtil
{
public:
    static std::string MakeKeyStr(std::string key, rsjquotestyletype quoteStyle = qsDOUBLE)
    {
	std::string s = quoteStyle == qsSINGLE ? "'" : quoteStyle == qsDOUBLE ? "\"" : "";

	if (quoteStyle == qsBEST) { quoteStyle = qsDOUBLE; }

	int len = key.length();

	for(int i = 0; i < len; ++i)
	{
	    if (quoteStyle == qsSINGLE)
	    {
		if (key[i] == '\'') //we found a single quote before the end of the string
		{
		    //escape the single quote (ie. ' i can\'t do anything ')
		    s += "\\'";
		    continue;
		}
	    }
	    else if (quoteStyle == qsDOUBLE)
	    {
		if (key[i] == '"')
		{
		    //escape the double quote
		    s += "\\\"";
		    continue;
		}
	    }

	    //convert actual newlines and backslashes into representations
	    switch(key[i])
	    {
	    case '\\': s += "\\\\"; break;
	    case '\b': s += "\\b"; break;
	    case '\f': s += "\\f"; break;
	    case '\n': s += "\\n"; break;
	    case '\r': s += "\\r"; break;
	    case '\t': s += "\\t"; break;

	    default:
		s += key[i];
	    }
	}

	s += quoteStyle == qsSINGLE ? "'" : quoteStyle == qsDOUBLE ? "\"" : "";
	return s;
    }

};

enum RSJsonType { rstInvalid, rstObject, rstArray, rstString, rstFloat, rstInt };

#include "RSJsonInt.h"
#include "RSJsonFloat.h"
#include "RSJsonString.h"
#include "RSJsonArray.h"
#include "RSJsonObject.h"


#endif
