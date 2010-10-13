#ifndef RSJSONBASIC_H
#define RSJSONBASIC_H

#include "RSJson.h"

#include <string>
#include <stdexcept>

using namespace std;

class RSJsonBasic
{
protected:
//use this to store encoded string
    std::string m_encStr;
public:
//encode this (or derived) object to a json string
    virtual const std::string& AsJsonString() = 0;


    virtual RSJsonType Type() const {
	return rstInvalid;
    }

    RSJsonBasic() : m_encStr("") {
    }
    virtual ~RSJsonBasic(){
    }

    virtual int IntVal() const {
	throw std::runtime_error("Cannot convert to int");
    }
    virtual float FloatVal() const {
	throw std::runtime_error("Cannot convert to float");
    }
    virtual const std::string& StringVal(){
	  switch(this->Type()){
		  case rstObject: return (m_encStr = "Object{}");
		  case rstArray: return (m_encStr = "Array[]");
		  default:
		     throw std::runtime_error("Cannot convert to std::string");
	  }
    };

    virtual RSJsonBasic* get(const std::string& key){
	return NULL;
    }
    virtual RSJsonBasic* get(int i){
	return NULL;
    }

    //javascript object Get dot notation (ie. dget("myobject.foo.bar.value") )
    //should convert to myobj->get("foo")->get("bar")->get("value")
    virtual RSJsonBasic* dget(const std::string& js){
		DEBUG_PRINTF("JS DOT Notation dget: %s\n", js.c_str());
		try{
			int len = (int)js.length();
			string s;
			RSJsonBasic* temp = this;
			for (int i = 0; i < len; ++i){
				if (js[i] == '.'){
					temp = temp->get(s);
					s = "";
				}
				else{
					s += js[i];
				}
			}
			return temp->get(s);
	    }
	    catch(...){ return NULL; }
	}
};

#endif
