#ifndef RADIUMENGINE_ASSET_DATA_HPP
#define RADIUMENGINE_ASSET_DATA_HPP

#include <Core/RaCore.hpp>
#include <string>

namespace Ra {
namespace Asset {

/// The AssetData class is a generic class for containers storing data collected from a loaded file.
class RA_CORE_API AssetData {
  public:
    AssetData( const std::string& name ) : m_name( name ) {}

    virtual ~AssetData() {}

    /// Return the name of the object's data.
    inline virtual const std::string& getName() const { return m_name; }

  protected:
    /// The name of the object's data.
    std::string m_name;
};

} // namespace Asset
} // namespace Ra

#endif // RADIUMENGINE_GEOMETRY_DATA_HPP
