#include "ability_name.h"

AbilityName* AbilityName::_instance = nullptr;

AbilityName::AbilityName()
{

}

AbilityName *AbilityName::get_instance()
{
    if (!_instance) {
        _instance = new AbilityName();
        _instance->add_property(PROPERTY_NAME_SLIDE, "Slide");
    }
    return _instance;

}

void AbilityName::add_property(e_PROPERTIES_NAMES number, std::string name)
{
    property_map.insert(std::pair<int, std::string>(number, name));
}


std::string AbilityName::get_name_from_number(int number)
{
    return property_map.at(number);
}



