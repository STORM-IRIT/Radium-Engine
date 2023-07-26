#pragma once

#include <Engine/Data/TextureManager.hpp>
#include <Engine/RadiumEngine.hpp>

namespace Ra {
namespace Engine {
namespace Data {

/** @brief Base class to manage a set of textures indexed by semantic (enum).
 */
template <typename TextureSemantic>
class MaterialTextureSet
{
  public:
    virtual ~MaterialTextureSet() = default;

    void addTexture( const TextureSemantic& semantic,
                     const TextureManager::TextureHandle& texture ) {
        m_textures[semantic] = texture;
    }

    void addTexture( const TextureSemantic& semantic, const TextureParameters& texture ) {
        auto texManager = RadiumEngine::getInstance()->getTextureManager();
        addTexture( semantic, texManager->addTexture( texture ) );
    }

    Texture* getTexture( const TextureSemantic& semantic ) const {
        Texture* tex    = nullptr;
        auto texManager = RadiumEngine::getInstance()->getTextureManager();
        auto it         = m_textures.find( semantic );
        if ( it != m_textures.end() ) { tex = texManager->getTexture( it->second ); }
        return tex;
    }

  private:
    std::map<TextureSemantic, TextureManager::TextureHandle> m_textures;
};
} // namespace Data
} // namespace Engine
} // namespace Ra
