#include "shareddata.h"

SharedData* SharedData::_instance = nullptr;

SharedData::SharedData()
{

}

SharedData *SharedData::get_instance()
{
    if (!_instance) {
        _instance = new SharedData();
    }
    return _instance;
}




