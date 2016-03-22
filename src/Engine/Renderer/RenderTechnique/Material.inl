#include <Engine/Renderer/RenderTechnique/Material.hpp>

namespace Ra
{
    namespace Engine
    {

        inline const std::string& Material::getName() const
        {
            return m_name;
        }

        inline void Material::setKd( const Core::Color& kd )
        {
            m_kd = kd;
        }

        inline void Material::setKs( const Core::Color& ks )
        {
            m_ks = ks;
        }

        inline void Material::setNs( Scalar ns )
        {
            m_ns = ns;
        }

        inline void Material::addTexture( const TextureType& type, Texture* texture )
        {
            // FIXME(Charly): Check if already present ?
            m_textures[type] = texture;
        }

        inline TextureData& Material::addTexture( const TextureType& type, const std::string& texture )
        {
            CORE_ASSERT(!texture.empty(), "Invalid texture name");

            TextureData data;
            data.name = texture;

            return addTexture(type, data);
        }

        inline TextureData& Material::addTexture(const TextureType& type, const TextureData& texture)
        {
            m_pendingTextures[type] = texture;
            m_isDirty = true;

            return m_pendingTextures[type];
        }

        inline const Core::Color& Material::getKd() const
        {
            return m_kd;
        }

        inline const Core::Color& Material::getKs() const
        {
            return m_ks;
        }

        inline Scalar Material::getNs() const
        {
            return m_ns;
        }

        inline Texture* Material::getTexture( const TextureType& type ) const
        {
            Texture* tex = nullptr;

            auto it = m_textures.find( type );
            if ( it != m_textures.end() )
            {
                tex = it->second;
            }

            return tex;
        }

        inline void Material::setMaterialType( const MaterialType& type )
        {
            m_type = type;
        }

        inline const Material::MaterialType& Material::getMaterialType() const
        {
            return m_type;
        }

    }
} // namespace Ra
