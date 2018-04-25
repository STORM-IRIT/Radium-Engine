#include <Core/File/MaterialData.hpp>

#include <Core/Log/Log.hpp>

#include <algorithm>
#include <iterator>

namespace Ra {
namespace Asset {

////////////////
/// MATERIAL ///
////////////////

/// NAME
inline void MaterialData::setName( const std::string& name ) {
    m_name = name;
}

/// TYPE
inline std::string MaterialData::getType() const {
    return m_type;
}

inline void MaterialData::setType( const std::string& type ) {
    m_type = type;
}

} // namespace Asset
} // namespace Ra
