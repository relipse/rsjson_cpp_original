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

using namespace std;

enum rsjquotestyletype{ qsBEST, qsSINGLE, qsDOUBLE };

class RSJsonUtil
{
public:
	static std::string MakeKeyStr(std::string key, rsjquotestyletype quoteStyle = qsDOUBLE)
	{
		std::string s = quoteStyle == qsSINGLE ? "'" : quoteStyle == qsDOUBLE ? "\"" : "";

		if (quoteStyle == qsBEST){ quoteStyle = qsDOUBLE; }

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

enum RSJsonType{ rstInvalid, rstObject, rstArray, rstString, rstFloat, rstInt };

#include "RSJsonBasic.h"

class RSJsonInt : public RSJsonBasic
{
public:
	int intValue;

	RSJsonInt(int i = 0) : intValue(i){ }
	const std::string& toString(){ char buffer[25]; sprintf(buffer, "%d", intValue); return (m_strRepresentation = buffer); }
	virtual RSJsonType Type() const { return rstInt; }

};

class RSJsonFloat : public RSJsonBasic
{
public:
	float floatValue;

	RSJsonFloat(float f = 0.0) : floatValue(f){ }
	const std::string& toString(){ char buffer[25]; sprintf(buffer, "%f", floatValue); return (m_strRepresentation = buffer); }
    virtual RSJsonType Type() const { return rstFloat; }
};

class RSJsonString : public RSJsonBasic
{
public:
	std::string strValue;
	RSJsonString(const std::string& s = "") : strValue(s) { }
	const std::string& toString(){ return (m_strRepresentation = RSJsonUtil::MakeKeyStr(strValue, qsDOUBLE)); }
    virtual RSJsonType Type() const{ return rstString; }
};



#include "RSJsonArray.h"
#include "RSJsonObject.h"


#endif