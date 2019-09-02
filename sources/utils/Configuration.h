#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "StringEx.h"
#include "Directory.h"
#include <string>
#include <map>
#include <fstream>

using namespace std;

class Configuration
{
public:
    Configuration();
    ~Configuration();
    bool loadConfiguration(const std::string &configFile);
    std::string getValue(const std::string &section, const std::string &settingKey, const std::string defval="");
    bool isSection(const string &section);
private:
    void addSection(std::string &str, const std::map<std::string, std::string> &list);
    std::map<std::string, std::map<std::string, std::string> > _ConfigurationMap;
};

#endif // CONFIGURATION_H
