#pragma once

#include <Engine/RaEngine.hpp>

#include <map>
#include <string>

#include <Engine/Data/RenderParameters.hpp>
#include <Engine/Data/Texture.hpp>
#include <Engine/Data/TextureManager.hpp>
#include <Engine/RadiumEngine.hpp>

namespace Ra {
namespace Engine {
namespace Data {

/// Semantic of the texture : define which BSDF parameter is controled by the texture

/** @brief Base class to manage a set of textures indexed by semantic (enum).
 */
template <typename TextureSemantic>
class MaterialTextureSet
{
  public:
    void addTexture( const TextureSemantic& semantic,
                     const TextureManager::TextureHandle& texture ) {
        m_textures[semantic] = texture;
    }

    void addTexture( const TextureSemantic& semantic, const TextureParameters& texture ) {
        auto texManager      = RadiumEngine::getInstance()->getTextureManager();
        m_textures[semantic] = texManager->addTexture( texture );
    }

    Texture* getTexture( const TextureSemantic& semantic ) const {
        Texture* tex    = nullptr;
        auto texManager = RadiumEngine::getInstance()->getTextureManager();
        auto it         = m_textures.find( semantic );
        if ( it != m_textures.end() ) { tex = texManager->getTexture( it->second ); }
        return tex;
    }

    void addTexture( const TextureSemantic& semantic, const std::string& texture ) {
        CORE_ASSERT( !texture.empty(), "Invalid texture name" );
        auto texManager = RadiumEngine::getInstance()->getTextureManager();
        auto texHandle  = texManager->getTextureHandle( texture );
        if ( texHandle.isValid() ) { addTexture( semantic, texHandle ); }
        else {
            TextureParameters data;
            data.name          = texture;
            data.sampler.wrapS = GL_REPEAT;
            data.sampler.wrapT = GL_REPEAT;
            if ( semantic != TextureSemantic::TEX_NORMAL ) {
                data.sampler.minFilter = GL_LINEAR_MIPMAP_LINEAR;
            }
            addTexture( semantic, data );
        }
    }

  private:
    std::map<TextureSemantic, TextureManager::TextureHandle> m_textures;
};

/**
 * Base class for materials/
 * Do not assume a given Material representation but only make the difference between opaque and
 * transparent materials.
 *
 */
class RA_ENGINE_API Material : public Data::ShaderParameterProvider
{
  public:
    /**
     * Identifies the type of the material.
     *  MAT_OPAQUE and MAT_TRANSPARENT implements the GLSL "surfacic" BSDF interface
     *  MAT_DENSITY implements the GLSL "volumetric" interface
     */
    enum class MaterialAspect {
        MAT_OPAQUE,      /// <- The material is either OPAQUE or TRANSPARENT
        MAT_TRANSPARENT, /// <- The material is TRANSPARENT
        MAT_DENSITY      /// <- The material implements the VOLUME interface
    };

  public:
  protected:
    /**
     * Creates a named material with the given aspect
     * \param instanceName
     * \param materialName
     * \param aspect
     */
    explicit Material( const std::string& instanceName,
                       const std::string& materialName,
                       MaterialAspect aspect = MaterialAspect::MAT_OPAQUE );

    /** Change the Material Name
     * \note This method should be used carefully as the name is a key for render technique factory
     */
    inline void setMaterialName( std::string newName ) { m_materialName = std::move( newName ); }

  public:
    virtual ~Material() = default;

    /**
     * \return the name of the material instance
     */
    inline const std::string& getInstanceName() const { return m_instanceName; }

    /**
     * \return the name of the material, can be used a UUID
     * \note the material name is expected to be used to define the ShaderConfiguration name
     */
    inline const std::string& getMaterialName() const { return m_materialName; }

    /** set the aspect (MAT_OPAQUE or MAT_TRANSPARENT) of the material.
     * \param aspect
     */
    inline void setMaterialAspect( const MaterialAspect& aspect ) { m_aspect = aspect; }

    /** Get the aspect (MAT_OPAQUE or MAT_TRANSPARENT) of the material.
     *
     * \return the current aspect of the Material
     */
    inline const MaterialAspect& getMaterialAspect() const { return m_aspect; }

    /** Test if material is transperent.
     * \return true if the material is transparent
     */
    virtual bool isTransparent() const;

    /**
     * Get the list of properties the material migh use in a shader.
     * each property will be added to the shader used for rendering this material under the form
     * "#define theProperty". Shaders that support the given property could then fully render the
     * material. Others migh render the meterial eroneously.
     *
     * The default implementation returns an empty list.
     *
     * \todo : Validate this proposal
     * \todo : make the property list modifiable as well
     */
    std::list<std::string> getPropertyList() const override;

    /**
     * \brief Makes the Material take its base color from the VERTEX_COLOR attribute of the rendered
     * geometry \param state activate (true) or deactivate (false) VERTEX_COLOR attribute usage
     *
     * Any material that support per-vertex color parameterization should implement this method
     * accordingly
     */
    virtual void setColoredByVertexAttrib( bool /* state */ ) {};

    /**
     * \brief Indicates if the material takes the VERTEX_COLOR attribute into account.
     */
    virtual bool isColoredByVertexAttrib() const { return false; }

    /** Mark the Material as needing update before the next OpenGL call
     *
     */
    inline void needUpdate() { m_isDirty = true; }

  protected:
    /// Material instance name
    std::string m_instanceName {};
    /// Material aspect
    MaterialAspect m_aspect { MaterialAspect::MAT_OPAQUE };
    /// Dirty mark : true if the openGL state of the material need to be updated before next draw
    /// call
    bool m_isDirty { true };

  private:
    /// Unique material name that can be used to identify the material class
    std::string m_materialName;
};

} // namespace Data
} // namespace Engine
} // namespace Ra
