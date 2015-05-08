#include "../interface/MCPMap.h"

std::map<std::string,int> MCPList;
std::map<int,std::string> inverted_MCPList;
   
void Fill_MCPList (void)
    {
      MCPList.insert(std::make_pair("MiB2",0));
      MCPList.insert(std::make_pair("ZStack1",1));
      MCPList.insert(std::make_pair("enSEE",2));
      MCPList.insert(std::make_pair("ZStack2",3));
      MCPList.insert(std::make_pair("MiB3",4));
      MCPList.insert(std::make_pair("Off1",5));
      MCPList.insert(std::make_pair("Off2",6));
      MCPList.insert(std::make_pair("Off3",7));
      MCPList.insert(std::make_pair("Off4",8));
      MCPList.insert(std::make_pair("clock1",9));
      MCPList.insert(std::make_pair("clock2",10));
      MCPList.insert(std::make_pair("Off6",11));
      MCPList.insert(std::make_pair("Off7",12));
      MCPList.insert(std::make_pair("Off8",13));
      MCPList.insert(std::make_pair("Off9",14));
      MCPList.insert(std::make_pair("Off10",15));
      MCPList.insert(std::make_pair("Off11",16));
      MCPList.insert(std::make_pair("Off12",17));
    }

void Fill_inverted_MCPList (void)
    {
      inverted_MCPList.insert(std::make_pair(0,"MiB2"));
      inverted_MCPList.insert(std::make_pair(1,"ZStack1"));
      inverted_MCPList.insert(std::make_pair(2,"enSEE"));
      inverted_MCPList.insert(std::make_pair(3,"ZStack2"));
      inverted_MCPList.insert(std::make_pair(4,"MiB3"));
      inverted_MCPList.insert(std::make_pair(5,"Off1"));
      inverted_MCPList.insert(std::make_pair(6,"Off2"));
      inverted_MCPList.insert(std::make_pair(7,"Off3"));
      inverted_MCPList.insert(std::make_pair(8,"Off4"));
      inverted_MCPList.insert(std::make_pair(9,"clock1"));
      inverted_MCPList.insert(std::make_pair(10,"clock2"));
      inverted_MCPList.insert(std::make_pair(11,"Off6"));
      inverted_MCPList.insert(std::make_pair(12,"Off7"));
      inverted_MCPList.insert(std::make_pair(13,"Off8"));
      inverted_MCPList.insert(std::make_pair(14,"Off9"));
      inverted_MCPList.insert(std::make_pair(15,"Off10"));
      inverted_MCPList.insert(std::make_pair(16,"Off11"));
      inverted_MCPList.insert(std::make_pair(17,"Off12"));
    }
