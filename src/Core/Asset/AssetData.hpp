#pragma once
#include <Core/RaCore.hpp>

#include <string>

namespace Ra {
namespace Core {
namespace Asset {

/**
 * General interface of an asset data.
 *
 *  Every asset data must have a name and must be copy constructible.
 *
 */
class RA_CORE_API AssetData
{
  public:
    /// Construct an asset data given its name.
    explicit AssetData( const std::string& name ) : m_name( name ) {}

    /// Simple delete operator
    virtual ~AssetData() = default;

    /// Access to the name of the asset
    inline const std::string& getName() const { return m_name; }
    /// Set the name of the asset
    inline void setName( const std::string& name ) { m_name = name; }

  private:
    std::string m_name;
};

} // namespace Asset
} // namespace Core
} // namespace Ra
