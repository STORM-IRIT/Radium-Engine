#ifndef RADIUMENGINE_ASSET_DATA_HPP
#define RADIUMENGINE_ASSET_DATA_HPP

#include <Core/RaCore.hpp>
#include <string>

namespace Ra {
namespace Asset {

/**
 * General interface of an asset data.
 *
 *  Every asset data must have a name and must be copy constructible.
 *
 */
class RA_CORE_API AssetData {
  public:
    /// Construct an asset data given its name.
    AssetData( const std::string& name ) : m_name( name ) {}

    /// Copy constructor. Default here
    AssetData (const AssetData &other) = default;

    /// Simple delete operator
    virtual ~AssetData() {}

    /// Acces to the name of the asset
    inline virtual const std::string& getName() const { return m_name; }

  protected:
    std::string m_name;
};

} // namespace Asset
} // namespace Ra

#endif // RADIUMENGINE_GEOMETRY_DATA_HPP
