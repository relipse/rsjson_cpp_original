#ifndef RSJSONOBJECT_H
#define RSJSONOBJECT_H

#include "RSJsonBasic.h"

typedef map<string, RSJsonBasic*> rsjsonmap;

class RSJsonObject : public RSJsonBasic
{
public:
	rsjsonmap m_obj;
public:
	inline RSJsonBasic*& operator[](const std::string& key)
	{
		return m_obj[key];
	}

	//RSJsonBasic* L(const std::string& key){ RSJsonBasic* o = exists(key); if (!o){ m_obj[key] = NULL; } }


	void set(const std::string& key, RSJsonBasic* value)
	{
	    //if it already exists, we are going to have to delete the existing value
		if (exists(key) && m_obj[key] != value)
		{
		   delete m_obj[key]; //free the memory   
		}
		m_obj[key] = value;
	}

	
	virtual RSJsonBasic* get(const std::string& key){ return exists(key); }

	//return NULL if does not exist, otherwise return the pointer to the object
	RSJsonBasic* exists(const std::string& key)
	{
		rsjsonmap::iterator iter = m_obj.find(key);
		if (iter != m_obj.end()){ return iter->second; }
		else { return NULL; }
	}

	inline int count()
	{
		return (int)m_obj.size();
	}

	//TODO: fix this, it produces some really whacky results
	const std::string& toString()
	{
		string &s = m_strRepresentation;
		s = "{";
		if (m_obj.size() > 0)
		{
			for (rsjsonmap::reverse_iterator iter = m_obj.rbegin(); iter != m_obj.rend(); iter++)
			{
				if (iter != m_obj.rbegin()){ s += ",\r\n"; }
				s += RSJsonUtil::MakeKeyStr(iter->first) + ":" + iter->second->toString();
			}
		}
		s += "}";
		return s;
	}

	//TODO: fix this destructor, it totally messes up!
	virtual ~RSJsonObject()
	{
		for (rsjsonmap::iterator iter = m_obj.begin(); iter != m_obj.end(); ++iter)
		{
			delete iter->second;
			iter->second = NULL;
		}
	}

	virtual RSJsonType Type() const { return rstObject; }
};


#endif