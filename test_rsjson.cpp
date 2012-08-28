#include "RSJson.h"
#include "RSJsonParser.h"

int ReplenishBuffer(char*& buffer, int bufferLength)
{
   return 0;   
}

#include <string>
#include <iostream>

int main(int argc, char** argv)
{
   std::string s("{phil:26, jim: 27, chris: 22, amanda: 18}");
   RSJsonParser parser;
   RSJsonObject* json = NULL; 
   
   cout << "Type \"EXIT\" to quit." << endl;
   cout << "Type some json:" << endl;
   getline (cin, s);
   
   json = parser.Parse(s);
   if (json == NULL){
      cout << "Invalid json code" << endl;        
      cin >> s;
      return 1;
   }
   
   while (s != "EXIT"){
      cout << "Enter a key to see:" << endl;
      getline (cin, s);   
      if (s == "EXIT"){
        break;     
      } 
      RSJsonBasic* kv = json->get(s);
      if (kv){   
          std::cout << kv->StringVal() << endl; 
      }else{
          std::cout << "Invalid key!" << endl;     
      }
   }
   
   return 0;
}
