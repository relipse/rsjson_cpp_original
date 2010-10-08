#ifndef RSJSONBASIC_H
#define RSJSONBASIC_H

#include "RSJson.h"

class RSJsonBasic
{
protected:
    std::string m_strRepresentation;
public:
	virtual const std::string& toString(){ return m_strRepresentation; }
	//virtual void LoadFromString(std::string) = 0;
	virtual RSJsonType Type() const { return rstInvalid; }

	virtual ~RSJsonBasic(){ }


	virtual RSJsonBasic* get(const std::string& key){ return NULL; }
	virtual RSJsonBasic* get(int i){ return NULL; }
};

#endif 