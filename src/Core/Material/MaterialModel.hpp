#pragma once
#include <string>

#include <Core/RaCore.hpp>

#include <Core/Utils/Observable.hpp>

namespace Ra {
namespace Core {
namespace Material {

/** @brief represent material model, loaded by a file loader.
 *
 */
class RA_CORE_API MaterialModel : public Utils::ObservableVoid
{
  public:
    /// MATERIAL DATA
    MaterialModel( const std::string& name = "", const std::string& type = "AbstractMaterial" );
    virtual ~MaterialModel() { detachAll(); };

    /// NAME
    inline const std::string& getName() const { return m_name; }
    inline void setName( const std::string& name ) { m_name = name; }

    /// TYPE
    inline std::string getType() const { return m_materialType; }
    inline void setType( const std::string& type ) { m_materialType = type; }

    /// DEBUG
    virtual void displayInfo() const;

  private:
    std::string m_materialType;
    std::string m_name;
};

/**
 * \brief Convenient alias for material usage
 */

using MaterialModelPtr = std::shared_ptr<MaterialModel>;

inline MaterialModel::MaterialModel( const std::string& name, const std::string& type ) :
    m_materialType { type }, m_name { name } {}

} // namespace Material
} // namespace Core
} // namespace Ra
