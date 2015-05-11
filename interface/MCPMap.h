#ifndef __MCPMap__
#define __MCPMap__

#include<iostream>
#include<map>

extern std::map<std::string,int> MCPList;
extern std::map<int,std::string> inverted_MCPList;
   
void Fill_MCPList (void);

void Fill_inverted_MCPList (void);

#endif
