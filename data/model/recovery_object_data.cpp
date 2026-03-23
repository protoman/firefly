#include "recovery_object_data.h"

RecoveryObjectData::RecoveryObjectData()
    : ObjectData(),
      m_type(RecoveryObjectDataTypeEnum::E_TANK) {
}

RecoveryObjectDataTypeEnum RecoveryObjectData::get_type() const {
    return m_type;
}

void RecoveryObjectData::set_type(RecoveryObjectDataTypeEnum type) {
    m_type = type;
}
