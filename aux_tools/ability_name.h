#ifndef PROPERTIESNAMES_H
#define PROPERTIESNAMES_H

#include <iostream>
#include <string>
#include <map>

#include "defines.h"

class AbilityName
{
public:
    static AbilityName* get_instance();
    std::string get_name_from_number(int number);

private:
    AbilityName();
    std::map<int, std::string> property_map;
    void add_property(e_PROPERTIES_NAMES number, std::string name);
    static AbilityName* _instance;
};


#endif // PROPERTIESNAMES_H
