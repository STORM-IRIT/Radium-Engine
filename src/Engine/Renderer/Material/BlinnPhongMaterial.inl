#include <Engine/Renderer/Material/BlinnPhongMaterial.hpp>


namespace Ra {
  namespace Engine {

    inline void BlinnPhongMaterial::addTexture(const TextureType &type, Texture *texture)
    {
        m_textures[type] = texture;
    }

    inline TextureData &BlinnPhongMaterial::addTexture(const TextureType &type, const std::string &texture)
    {
        CORE_ASSERT(!texture.empty(), "Invalid texture name");

        TextureData data;
        data.name = texture;
        data.wrapS = GL_REPEAT;
        data.wrapT = GL_REPEAT;
        data.magFilter = GL_NEAREST;
        data.minFilter = GL_NEAREST;

        return addTexture(type, data);
    }

    inline TextureData &BlinnPhongMaterial::addTexture(const TextureType &type, const TextureData &texture)
    {
        m_pendingTextures[type] = texture;
        m_isDirty = true;

        return m_pendingTextures[type];
    }

    inline Texture *BlinnPhongMaterial::getTexture(const TextureType &type) const
    {
        Texture *tex = nullptr;

        auto it = m_textures.find(type);
        if (it != m_textures.end())
        {
            tex = it->second;
        }

        return tex;
    }
  }
}

