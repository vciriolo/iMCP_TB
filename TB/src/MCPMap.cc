#include "../interface/MCPMap.h"
   
void Fill_MCPList (void)
    {
      MCPList.insert(std::make_pair("MiB2",0));
      MCPList.insert(std::make_pair("ZStack1",1));
      MCPList.insert(std::make_pair("EnSEE",2));
      MCPList.insert(std::make_pair("ZStack2",3));
      MCPList.insert(std::make_pair("MiB3",4));
      MCPList.insert(std::make_pair("Off1",5));
      MCPList.insert(std::make_pair("Off2",6));
      MCPList.insert(std::make_pair("Off3",7));
      MCPList.insert(std::make_pair("Off4",8));
    }

void Fill_inverted_MCPList (void)
    {
      inverted_MCPList.insert(std::make_pair(0,"MiB2"));
      inverted_MCPList.insert(std::make_pair(1,"ZStack1"));
      inverted_MCPList.insert(std::make_pair(2,"EnSee"));
      inverted_MCPList.insert(std::make_pair(3,"ZStack2"));
      inverted_MCPList.insert(std::make_pair(4,"MiB3"));
      inverted_MCPList.insert(std::make_pair(5,"Off1"));
      inverted_MCPList.insert(std::make_pair(6,"Off2"));
      inverted_MCPList.insert(std::make_pair(7,"Off3"));
      inverted_MCPList.insert(std::make_pair(8,"Off4"));
    }
