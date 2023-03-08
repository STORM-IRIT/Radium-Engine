#pragma once
#include <string>
#include <vector>

#include <Core/Asset/AssetData.hpp>
#include <Core/RaCore.hpp>

#include <Core/Material/MaterialModel.hpp>

namespace Ra {
namespace Core {
namespace Asset {

/** \brief represent material data loaded by a file loader.
 * \todo rewrite the doc
 * Material data must be identified by a unique name.
 * Radium Engine reserves the following names
 *  "AbstractMaterial"  --> unknown material, might serve for error management
 *  "BlinnPhong"        --> standar Blinn-Phong Material
 *
 *  When extending the material system with a loader plugin (or something similar) :
 *      Define your own "Asset" class derived from Ra::Core::Asset::MaterialData with the "type"
 * that identifies it uniquely and corresponding to the external format of your material (in the
 * file you load). Define your own "Engine" class, derived from Ra::Engine::Material.
 *
 *      Make your plugin register the converter function from "Asset" to "Engine" for this material.
 *          This function may be everithing that is of the same type than
 * std::function<RadiumMaterialPtr(AssetMaterialPtr)> Make your plugin register the default
 * technique builder for this material. This will require to write some GLSL shaders and make them
 * accessible from the application search directory. See example in RadiumEngine.cpp
 * (RadiumEngine::initialize()) that defins default BlinnPhong Material.
 *
 *      Make your loader instantiate the right "Asset" MaterialData class while loading material
 * data from a file.
 *
 *      The active system (GeometrySystem is the Radium Default), will then automatically use your
 * new material and technique so that the rendering will be fine. When writing your own system, see
 * GeometrySystem implementation as an example
 */
class RA_CORE_API MaterialData : public AssetData
{
  public:
    /// MATERIAL DATA
    MaterialData( const std::string& name = "" ) : AssetData( name ) {}
    ~MaterialData() = default;

    /// NAME
    void setName( const std::string& name ) { m_name = name; }

    /// DEBUG
    void displayInfo() const {
        if ( m_material ) { m_material->displayInfo(); }
    }

    /// Model
    void setMaterialModel( Material::MaterialModelPtr model ) { m_material = model; }
    Material::MaterialModelPtr getMaterialModel() const { return m_material; }

  private:
    Material::MaterialModelPtr m_material;
};

} // namespace Asset
} // namespace Core
} // namespace Ra
