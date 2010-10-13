/***********************************
* This is a Really Simple JSON parser
* It does not support expressions or
* variables, only keys and values.
* There are no key words
*
* Author: Jim A Kinsman
* Date: Oct 5, 2010
   Comments:
   Here is the BNF for Really Simple JSON

        <json> => <object>
        <object> => '{' <key-value-list> '}'
        <array> => '[' <csv-list> ']'
        <csv-list> => <value> [',' <csv-list> ]
        <key-value-list> => (<variable> | <string-literal> ) ':' <value> [ ','   <key-value-list> ]
        <variable> => [a-z]([a-z]|[0-9]|_)*
        <string-literal> => '"' ANYCHAR* '"' | ' ANYCHAR* '
        <value> => <object> | <array> | INT | STRING-LITERAL | FLOAT | BOOL


        <phpstyleget>=> <variable>'['<string-literal>']'

***********************************/
#ifndef RSJSONPARSER_H
#define RSJSONPARSER_H

//store the objects being parsed
#include "RSJson.h"

#include <ctype.h>
#include <vector>
#include <string>

//use this for atoi and atof
#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
using namespace std;



char rsjsonpuncts[] = { '{', '}', '[', ']', ':', '"', '\'', ',' };

enum rsjsontype {rjtOPEN_BRACE, rjtCLOSE_BRACE,
                 rjtOPEN_BRACKET, rjtCLOSE_BRACKET,
                 rjtCOLON, rjtDOUBLE_QUOTE, rjtSINGLE_QUOTE, rjtCOMMA,
                 rjtSTRING_LITERAL, rjtVARIABLE, rjtSTRING, rjtINT, rjtFLOAT, rjtBOOL,
                 rjtEOF, rjtUNKNOWN, rjtCOMMENT};

char* rsjsontypes[] = {"OPEN_BRACE '{'", "CLOSE_BRACE '}'",
                       "OPEN_BRACKET '['", "CLOSE_BRACKET ']'", "COLON ':'",
                       "DOUBLE_QUOTE '\"", "SINGLE_QUOTE \"'\"", "COMMA ','",
                       "STRING LITERAL 'blah' or \"blah\" ", "VARIABLE", "STRING",
                       "INTEGER", "FLOATING POINT", "BOOLEAN 'true'|'false'", "EOF"};



class RSJsonToken
{
public:
    string lexeme;
    rsjsontype tokenType;
};



class RSJsonParser
{
private:
    RSJsonToken curToken;
    RSJsonToken peekToken;
    std::vector<RSJsonToken> tokenStack; //eatnexttoken will take off of stack if it exists

    std::vector<string> keyStack;

    RSJsonObject* root;
    std::vector<RSJsonBasic*> rsJsonObjStack;


    char* buffer;
    int buffer_len;
    char* cur_char;

    int cur_line_num;
    int cur_column_num;


//a callback to replenish the buffer (in case we want to read from a file)
//for example:
// int ReplenishBuffer(char*& buffer, int bufferLength){ return bytesRead; }
    int (*replenishBuffer)(char*& /*buffer*/, int /*bufferlength*/);

    char peekbuffer[5];
    int peekbuffer_len;

    void replenish()
    {
	int bytesRead = replenishBuffer(buffer, buffer_len);
	buffer_len = bytesRead;

	if (bytesRead > 0)
	{
	    cur_char = buffer;
	}
    }


//keep track of line numbers etc..
    void nextchar()
    {
	cur_char++;

	//Replenish the buffer if needbe
	if ( (*cur_char == '\0' || *cur_char == EOF) && replenishBuffer != NULL)
	{
	    replenish(); //we cannot really undo this
	}

	cur_column_num++;
	if (*cur_char == '\n')
	{
	    cur_line_num++;
	    cur_column_num = 1; //reset column #
	}
    }

    char& curchar()
    {
	return *cur_char;
    }

    char peekchar()
    {
	//TODO: FIX THIS-> it breaks for ReplenishBuffer because it may be at the end of the buffer
	return *(cur_char+1);
    }

    string FetchRestOfVariable()
    {
	nextchar();

	string s;
	while((*cur_char) == '_' || isalnum(*cur_char))
	{
	    s += (*cur_char);
	    nextchar();
	}
	return s;
    }

    string FetchRestOfDigit(bool& isfloat)
    {
	isfloat = false;
	nextchar();

	string s;
	while(*cur_char == '.' || isdigit(*cur_char))
	{
	    s += (*cur_char);

	    if (*cur_char == '.') {
		//we know it is a float now!!!
		if (isfloat) {
		    throw std::runtime_error("multiple periods ('.') found, supposed to be a float?");
		}
		isfloat = true;
	    }
	    nextchar();
	}
	return s;
    }


//startendchar is a single or double quote
//TODO: span multiple lines?
    string FetchRestOfStringLiteral(char startendchar)
    {
	nextchar();
	string s;
	while(*cur_char != startendchar)
	{
	    if (*cur_char == '\\') { nextchar(); }
	    s += (*cur_char);
	    nextchar();
	}
	//skip the final quote (single or double)
	nextchar();
	return s;
    }

//eat next token, return current, place that original token back on stack
    RSJsonToken Peek()
    {
	RSJsonToken temp = curToken;
	EatNextToken();
	peekToken = curToken;
	tokenStack.push_back(curToken);
	curToken = temp; //restore original curToken value
	return peekToken;
    }
    

    void skipwhitespace(){      
	//skip whitespace
	while (isspace(*cur_char)) //|| *cur_char == 'Â')
	{
	    nextchar();
	}
    }


    void EatNextToken()
    {
	if (tokenStack.size() > 0) {
	    curToken = tokenStack.back();
	    tokenStack.pop_back();
	    return;
	}

        skipwhitespace();

	char c = *cur_char;

	//skip comments
	if (*cur_char == '/' && peekchar()=='/') {
	    DEBUG_PRINTF("// comment detected, attempting to skip until newline or EOF\n");
	    while(*cur_char != '\n' && *cur_char != EOF && *cur_char != '\0') { nextchar(); }
            skipwhitespace();
	    c = *cur_char;
	}
	if (*cur_char == '/' && peekchar()=='*') {
	    DEBUG_PRINTF("/*/ comment detected, attempting to skip until */ or EOF\n");
	    nextchar();
	    while(!(*cur_char == '*' && peekchar() == '/'))
            {
               if (*cur_char == '\0' || *cur_char == EOF){ 
                    curToken.tokenType = rjtEOF;
                    return;
               }
               DEBUG_PRINTF("%c", *cur_char);
               nextchar();
            }
            nextchar();
            nextchar();

            DEBUG_PRINTF("\nchar after comment loop done: %c\n", *cur_char);
	    skipwhitespace();
	    c = *cur_char;
	}
	//initially set the new lexeme
	curToken.lexeme = c;

	switch (c) {

	case '{': curToken.tokenType = rjtOPEN_BRACE; break;
	case '}': curToken.tokenType = rjtCLOSE_BRACE; break;
	case '[': curToken.tokenType = rjtOPEN_BRACKET; break;
	case ']': curToken.tokenType = rjtCLOSE_BRACKET; break;
	case ':': curToken.tokenType = rjtCOLON; break;
	case ',': curToken.tokenType = rjtCOMMA; break;
	//case '"': curToken.tokenType = rjtDOUBLE_QUOTE; break;
	//case '\'': curToken.tokenType = rjtSINGLE_QUOTE; break;
	case '\0': curToken.tokenType = rjtEOF; break;
	default:
	    if (c == '_' || isalpha(c))
	    {
		curToken.tokenType = rjtVARIABLE;
		curToken.lexeme += FetchRestOfVariable();


		return;
	    }
	    //two types of string literals: "blah" or 'blah'
	    else if (c == '"' || c == '\'')
	    {
		curToken.tokenType = rjtSTRING;
		curToken.lexeme = FetchRestOfStringLiteral(c);
		return;
	    }
	    //allow negative numbers
	    else if (c == '-' || isdigit(c))
	    {
		//either a float or an integer
		bool isfloat;
		curToken.tokenType = rjtUNKNOWN;
		curToken.lexeme += FetchRestOfDigit(isfloat);
		curToken.tokenType = isfloat ? rjtFLOAT : rjtINT;
		return;
	    }
	    else     //invalid character, just assign it as an UNKNOWN
	    {
		curToken.tokenType = rjtUNKNOWN;
	    }
	}
	//go to next char
	nextchar();
    }

    void Match(rsjsontype tokenType)
    {
	//DEBUG_PRINTF("Matching tokenType %s\n", rsjsontypes[tokenType]);
        EatNextToken();

	RSJsonToken& next = curToken;
	if (next.tokenType != tokenType)
	{
	    string s;
	    char buffer[15];
	    sprintf(buffer, "%d", cur_line_num);
	    s = "On Line " + string(buffer);
	    sprintf(buffer, "%d", cur_column_num);


	    s += " char " + string(buffer) + ": Expected " + string(rsjsontypes[tokenType]) +
	         " but received " + string(rsjsontypes[next.tokenType]) + " \"" + next.lexeme + "\"";
	    DEBUG_PRINTF("%s\n", s.c_str());
	    throw std::runtime_error(s.c_str());
	}
    }



public:
    RSJsonParser() : root(NULL), replenishBuffer(NULL) {
    }
    ~RSJsonParser()
    {
	delete root;
    }

    RSJsonObject* Parse(const std::string& s, int (*ReplenishBuffer)(char*& /*buffer*/, int /*bufferlength*/)  = NULL)
    {
	char* buff = new char[s.length()+1];
	strcpy(buff, s.c_str());
	buff[s.length()] = '\0';
	RSJsonObject* obj = Parse(buff, -1, ReplenishBuffer);
	delete[] buff;
	return obj;
    }


//basically allow anything to be parsed
    RSJsonBasic* ParseAnything(const std::string& s, int (*ReplenishBuffer)(char*& /*buffer*/, int /*bufferlength*/)  = NULL)
    {
	//just wrap in an object and parse away!!!
	//TODO: redo this function so we don't waste the extra resources
	string s2 = "{r:" + s + "}";
	Parse(s2, ReplenishBuffer);
	return root->get("r");
    }

//TODO: this does not work because peekchar() doesn't know what to do
    RSJsonObject* Parse(int (*ReplenishBuffer)(char*& /*buffer*/, int /*bufferlength*/))
    {
	//to allow for file reading or user input or whatever

	char temp[101] = "\0";
	char* p = temp;
	ReplenishBuffer(p, 101);
	return Parse(temp, 100, ReplenishBuffer);
    }


    RSJsonObject* Parse(char text[], int textSize = 0, int (*ReplenishBuffer)(char*& /*buffer*/, int /*bufferlength*/)  = NULL)
    {
	//to allow for file reading or user input or whatever
	replenishBuffer = ReplenishBuffer;


	//free up memory if we already used the parser
	delete root;
	root = NULL;

	buffer_len = textSize > 0 ? textSize : (int)strlen(text);
	buffer = text; //new char[buffer_len];

	if (buffer_len < 4)
	{
	    DEBUG_PRINTF("warning: buffer length is very short: %d chars\n", buffer_len);
	}

	//strcpy(buffer, text);
	cur_char = buffer;

	cur_line_num = 1;
	cur_column_num = 1;

	DEBUG_PRINTF("parsing: %s\n", buffer);
	try
	{
	    rsjson_language();
	    DEBUG_PRINTF("\nno errors.\n");
	    return root;
	}
	catch(exception& e)
	{
	    DEBUG_PRINTF("%s\n", e.what());
	    return NULL;
	}

    }

//the rsjson language always has a root object, therefore start with an object
//note* arrays can be values in the object
    void rsjson_language()
    {
	root = js_object();
    }

//a different type of rsjson parsing basically allowing anything:
//arrays, objects, integers, floats, or string literals
    RSJsonBasic* rsjson_easy_language()
    {
	root = new RSJsonObject();
	rsJsonObjStack.push_back(root);

	Peek();
	if (peekToken.tokenType == rjtOPEN_BRACE)
	{
	    root->set("value", js_object());
	}
	else if (peekToken.tokenType == rjtOPEN_BRACKET)
	{
	    root->set("value", js_array());
	}
	else //not an object or an array, hopefully its a value
	{
	    keyStack.push_back("value");
	    js_value();
	}
	return root->get("value");
    }



    RSJsonObject* js_object()
    {
	RSJsonObject* obj = NULL;
	try
	{
	    Match(rjtOPEN_BRACE);
	    obj = new RSJsonObject();
	    rsJsonObjStack.push_back(obj);

	    //allow empty objects
	    Peek();
	    if (peekToken.tokenType != rjtCLOSE_BRACE)
	    {
		js_keyvalue_list();
	    }
	    Match(rjtCLOSE_BRACE);
	    rsJsonObjStack.pop_back();
	    return obj;
	}
	catch(exception& e)
	{
	    delete obj;
	    throw e;
	}
    }

    RSJsonArray* js_array()
    {
	RSJsonArray* ary = NULL;
	try
	{
	    Match(rjtOPEN_BRACKET);
	    ary = new RSJsonArray();
	    rsJsonObjStack.push_back(ary);
	    //allow empty arrays (ie. "[]")
	    Peek();
	    if (peekToken.tokenType != rjtCLOSE_BRACKET)
	    {
		js_csvlist();
	    }
	    Match(rjtCLOSE_BRACKET);
	    rsJsonObjStack.pop_back();
	    return ary;
	}
	catch(exception& e)
	{
	    delete ary; //free memory
	    throw e;
	}
    }

    void js_csvlist()
    {
	js_value();
	keyStack.pop_back();

	Peek();
	if (peekToken.tokenType == rjtCOMMA)
	{
	    Match(rjtCOMMA);
	    js_csvlist();
	}
    }

//example: {"foo": "bar"} or {foo:"bar"} or {1:"bar"} or {1.26:"bar"} but not {-2:"bar"}
    void js_keyvalue_list()
    {
	string key;
	Peek();

	bool pushedKey = false;
	if (peekToken.tokenType == rjtVARIABLE || peekToken.tokenType == rjtSTRING ||

	    //allow positive integers and floats as key values (but not negative)
	    ((peekToken.tokenType == rjtINT ||
	      peekToken.tokenType == rjtFLOAT) &&
	     peekToken.lexeme[0] != '-')
	    )
	{
	    EatNextToken();
	    //store key
	    keyStack.push_back(curToken.lexeme);
	    pushedKey = true;
	}
	else
	{
	    throw std::runtime_error("expected key value (ie. \"mykey\", 'mykey2', or key: string literal or variable name");
	}

	Match(rjtCOLON);

	js_value();

	//we are done with the current key
	if (pushedKey) {
	    keyStack.pop_back();
	}

	Peek();
	if (peekToken.tokenType == rjtCOMMA)
	{
	    Match(rjtCOMMA);
	    js_keyvalue_list();
	}
    }


    RSJsonBasic* nwRSJsonBasicByType(const string& value, rsjsontype type)
    {
	switch(type)
	{
	case rjtSTRING:
	    return new RSJsonString(value);
	    break;
	case rjtINT:
	    return new RSJsonInt(atoi(value.c_str()));
	    break;
	case rjtFLOAT:
	    return new RSJsonFloat((float)atof(value.c_str()));
	    break;
	default:
	    return NULL;
	}
    }

/*
   string tabDepth()
   {
       string s = "";
       for (unsigned i = 0; i < rsJsonObjStack.size(); ++i)
       {
               s += "\t";
       }
       return s;
   }
 */


//return the level depth
    int countFullKey(std::string& fullkey)
    {
	string& s = fullkey;
	s = "";
	int i = 0;
	for (; i < (int)keyStack.size(); ++i) {
	    if (i > 0) { s+= '.'; }

	    s += keyStack[i];
	}
	return i; //return level depth
    }

    void appendValueToLastOnStack(RSJsonBasic* ptrRSJsonBasic)
    {
	if (rsJsonObjStack.back() == NULL)
	{
	    throw std::runtime_error("no object on the stack");
	}
	switch (rsJsonObjStack.back()->Type())
	{
	case rstObject:
	    if (keyStack.size() == 0) { keyStack.push_back("<undefined>"); }

	    (*(RSJsonObject*)rsJsonObjStack.back())[keyStack.back()] = ptrRSJsonBasic;
	    break;
	case rstArray:
	    ((RSJsonArray*)rsJsonObjStack.back())->push_back(ptrRSJsonBasic);
	    break;
	default:  throw std::runtime_error("this should never happen. last object on stack is not a RSJsonObject or RSJsonArray");
	}
    }


    void js_value()
    {
	Peek();
	if (peekToken.tokenType == rjtOPEN_BRACE)
	{
	    RSJsonObject* obj = js_object();
	    appendValueToLastOnStack(obj);
	    return;
	}
	if (peekToken.tokenType == rjtOPEN_BRACKET)
	{
	    RSJsonArray* ary = js_array();
	    appendValueToLastOnStack(ary);
	    return;
	}

	EatNextToken();

	char buff[15];
	int depth = -1;
	std::string fullkey;

	switch (curToken.tokenType)
	{
	case rjtSTRING:
	case rjtINT:
	case rjtFLOAT:
	    //do something with the data (store in last key on stack)

	    if (rsJsonObjStack.size() == 0)
	    {
		throw std::runtime_error("object/array stack is empty");
	    }
	    switch (rsJsonObjStack.back()->Type())
	    {
	    case rstObject:
		if (keyStack.size() == 0) { keyStack.push_back("<undefined>"); }

		(*(RSJsonObject*)rsJsonObjStack.back())[keyStack.back()] =
		    nwRSJsonBasicByType(curToken.lexeme, curToken.tokenType);
		break;
	    case rstArray:
		//keys are integers
		sprintf (buff, "%d", ((RSJsonArray*)rsJsonObjStack.back())->count());
		keyStack.push_back(buff);

		((RSJsonArray*)rsJsonObjStack.back())->push_back(
		    nwRSJsonBasicByType(curToken.lexeme, curToken.tokenType));
		break;
	    default: throw std::runtime_error("the object stack MUST contain RSJsonObject or RSJsonArray instance types! this should never occur");
	    }
	    depth = countFullKey(fullkey);
	    DEBUG_PRINTF("Depth: %d - Assigning \"%s\" = \"%s\" \n", depth, fullkey.c_str(), curToken.lexeme.c_str());
	    break;
	default:
	    throw std::runtime_error("expected string, int, or float");
	    break;
	}
    }


    RSJsonObject* GetRoot(){
	return (RSJsonObject*)root;
    }

};
#endif
