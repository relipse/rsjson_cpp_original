#ifndef RSJSONARRAY_H
#define RSJSONARRAY_H

#include "RSJsonBasic.h"

class RSJsonArray : public RSJsonBasic
{
	std::vector<RSJsonBasic*> m_ary;

public:
	virtual ~RSJsonArray()
	{
		for (int i = (int)m_ary.size()-1; i >= 0; --i)
		{
			delete m_ary[i];
			m_ary[i] = NULL;
		}
	}


	inline RSJsonBasic*& operator[](int i)
	{
		return m_ary[i];
	}

	virtual RSJsonBasic* get(int i){
		return m_ary[i];
	}

	inline void push_back(RSJsonBasic* item)
	{
		m_ary.push_back(item);
	}

	inline int count()
	{
		return (int)m_ary.size();
	}

	const std::string& AsJsonString()
	{
		m_encStr = "[";
		int count = this->count();
		for (int i = 0; i < count; ++i)
		{
			if (i != 0) { m_encStr += ",\r\n"; }
			m_encStr += m_ary[i]->AsJsonString();
		}
		m_encStr += "]";

		return m_encStr;
	}

	virtual RSJsonType Type() const {
		return rstArray;
	}
};


#endif
