#ifndef RSJSONSTRING_H
#define RSJSONSTRING_H

#include "RSJsonBasic.h"

class RSJsonString : public RSJsonBasic
{
public:
    std::string strValue;
    RSJsonString(const std::string& s = "") : strValue(s) {
    }
    const std::string& AsJsonString() {
	return (m_encStr = RSJsonUtil::MakeKeyStr(strValue, qsDOUBLE));
    }
    virtual RSJsonType Type() const {
	return rstString;
    }
    virtual const std::string& StringVal(){
	return strValue;
    }
};



#endif

