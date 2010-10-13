#ifndef RSJSONFLOAT_H
#define RSJSONFLOAT_H

#include "RSJsonBasic.h"

class RSJsonFloat : public RSJsonBasic
{
public:
    float floatValue;

    RSJsonFloat(float f = 0.0) : floatValue(f){
    }
    const std::string& AsJsonString() {
	char buffer[25]; sprintf(buffer, "%f", floatValue); return (m_encStr = buffer);
    }
    virtual RSJsonType Type() const {
	return rstFloat;
    }

    virtual int IntVal() const {
	return (int)(floatValue / 1);
    }
    virtual float FloatVal() const {
	return floatValue;
    }

    virtual const std::string& StringVal(){
	return AsJsonString();
    }
};


#endif

