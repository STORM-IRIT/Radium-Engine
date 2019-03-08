#ifndef RADIUMENGINE_ASSET_DATA_HPP
#define RADIUMENGINE_ASSET_DATA_HPP

#include <Core/RaCore.hpp>
#include <string>

namespace Ra {
namespace Core {
namespace Asset {

/**
 * General interface of an asset data.
 * Every asset data must have a name and must be copy constructible.
 */
class RA_CORE_API AssetData {
  public:
    AssetData( const std::string& name ) : m_name( name ) {}

    AssetData( const AssetData& other ) = default;

    virtual ~AssetData() {}

    /**
     * Acces to the name of the asset.
     */
    inline virtual const std::string& getName() const { return m_name; }

  protected:
    /// The name of the object's data.
    std::string m_name;
};

} // namespace Asset
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_GEOMETRY_DATA_HPP
