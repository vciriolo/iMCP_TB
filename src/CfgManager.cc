#include "../interface/CfgManager.h"

//**********getters***********************************************************************

//----------get option by name------------------------------------------------------------
template<> double CfgManager::GetOpt(string block, string key, int opt)
{
    double opt_val;
    istringstream buffer(opts_[block].at(key).at(opt));
    buffer >> opt_val;
    
    return opt_val;
}

template<> float CfgManager::GetOpt(string block, string key, int opt)
{
    return (float)GetOpt<double>(block, key, opt);
}

template<> int CfgManager::GetOpt(string block, string key, int opt)
{
    return (int)GetOpt<double>(block, key, opt);
}

template<> string CfgManager::GetOpt(string block, string key, int opt)
{
    return opts_[block].at(key).at(opt);
}    

//**********utils*************************************************************************

void CfgManager::ParseConfigFile(const char* file)
{
    ifstream cfgFile(file, ios::in);
    string buffer;
    string current_block="global";
    map<string, vector<string> > block_opts;
    while(cfgFile >> buffer)
    {
        if(buffer.at(0) == '#')
            continue;        
        istringstream splitter(buffer);
        vector<string> tokens = vector<string>(istream_iterator<string>(splitter), 
                                               istream_iterator<string>());
        if(tokens.at(0).at(0) == '<')
        {
            if(tokens.at(0).at(1) == '/')
            {
                tokens.at(0).erase(tokens.at(0).begin(), ++tokens.at(0).begin());
                tokens.at(0).erase(--tokens.at(0).end());
                if(tokens.at(0) == current_block)
                {
                    opts_[current_block] = block_opts;
                    block_opts.clear();
                    current_block = "global";
                }
                else
                    cout << "ERROR, cfg file: wrong closing block // " << tokens.at(0) << endl;
            }
            else
            {
                tokens.at(0).erase(tokens.at(0).begin());
                tokens.at(0).erase(--tokens.at(0).end());
                current_block = tokens.at(0);
            }
        }
        else
        {
            string key=tokens.at(0);
            tokens.erase(tokens.begin());
            block_opts[key] = tokens;
        }
    }
    cfgFile.close();
}

