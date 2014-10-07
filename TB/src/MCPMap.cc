#include "../interface/MCPMap.h"
   
void Fill_MCPList (void)
    {
      MCPList.insert(std::make_pair("MiB1",0));
      MCPList.insert(std::make_pair("Roma2",1));
      MCPList.insert(std::make_pair("MiB2",2));
      MCPList.insert(std::make_pair("MiB3",3));
      MCPList.insert(std::make_pair("Planacon",4));
      MCPList.insert(std::make_pair("Roma1",5));
      MCPList.insert(std::make_pair("Off2",6));
      MCPList.insert(std::make_pair("Off3",7));
      MCPList.insert(std::make_pair("Off4",8));
    }

void Fill_inverted_MCPList (void)
    {
      inverted_MCPList.insert(std::make_pair(0,"MiB1"));
      inverted_MCPList.insert(std::make_pair(1,"Roma2"));
      inverted_MCPList.insert(std::make_pair(2,"MiB2"));
      inverted_MCPList.insert(std::make_pair(3,"MiB3"));
      inverted_MCPList.insert(std::make_pair(4,"Planacon"));
      inverted_MCPList.insert(std::make_pair(5,"Roma1"));
      inverted_MCPList.insert(std::make_pair(6,"Off2"));
      inverted_MCPList.insert(std::make_pair(7,"Off3"));
      inverted_MCPList.insert(std::make_pair(8,"Off4"));
    }
