#ifndef RADIUMENGINE_MATERIAL_HPP
#define RADIUMENGINE_MATERIAL_HPP

#include <Engine/RaEngine.hpp>

#include <map>
#include <string>

#include <Core/Math/LinearAlgebra.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/Texture/TextureManager.hpp>

namespace Ra
{
  namespace Engine
  {
    class Texture;

    class ShaderProgram;
  }
}

namespace Ra
{
  namespace Engine
  {

    class RA_ENGINE_API Material
    {
    public:
        enum class TextureType
        {
            TEX_DIFFUSE,
            TEX_SPECULAR,
            TEX_NORMAL,
            TEX_SHININESS,
            TEX_ALPHA
        };

        // TODO : make Radium Material follow the diversity of Asset::MaterialData
        // This material could be the default : Blinn-Phong material
        enum class MaterialType
        {
            MAT_OPAQUE,
            MAT_TRANSPARENT
        };

    public:
        RA_CORE_ALIGNED_NEW

        explicit Material( const std::string& name );

        ~Material();

        void updateGL();

        void bind( const ShaderProgram* shader );

        const std::string& getName() const;

        void addTexture( const TextureType& type, Texture* texture );

        TextureData& addTexture( const TextureType& type, const std::string& texture );

        TextureData& addTexture( const TextureType& type, const TextureData& texture );

        Texture* getTexture( const TextureType& type ) const;

        void setMaterialType( const MaterialType& type );

        const MaterialType& getMaterialType() const;

    public:
        Core::Color m_kd;
        Core::Color m_ks;
        Scalar m_ns;
        Scalar m_alpha;

    private:
        std::string m_name;

        bool m_isDirty;

        std::map<TextureType, Texture*> m_textures;
        std::map<TextureType, TextureData> m_pendingTextures;

        MaterialType m_type;
    };

  } // namespace Engine
} // namespace Ra

#endif
