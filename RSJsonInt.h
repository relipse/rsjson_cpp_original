#ifndef RSJSONINT_H
#define RSJSONINT_H

#include "RSJsonBasic.h"

class RSJsonInt : public RSJsonBasic
{
public:
    int intValue;

    RSJsonInt(int i = 0) : intValue(i){
    }
    const std::string& AsJsonString() {
	char buffer[25]; sprintf(buffer, "%d", intValue); return (m_encStr = buffer);
    }
    virtual RSJsonType Type() const {
	return rstInt;
    }
    virtual int IntVal() const {
	return intValue;
    }
    virtual float FloatVal() const {
	return (int)intValue;
    }
    virtual const std::string& StringVal(){
	return AsJsonString();
    }
};

#endif

