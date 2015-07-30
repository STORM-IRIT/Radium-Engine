#ifndef RADIUMENGINE_TEXTUREMANAGER_HPP
#define RADIUMENGINE_TEXTUREMANAGER_HPP

#include <map>
#include <string>

#include <Core/Utils/Singleton.hpp>

namespace Ra { namespace Engine { class Texture; } }

namespace Ra { namespace Engine {

class RA_API TextureManager : public Core::Singleton<TextureManager>
{
private:
    friend class Core::Singleton<TextureManager>;
    typedef std::pair<std::string, Texture*> TexturePair;

public:
    Texture* addTexture(const std::string& filename);
    Texture* getOrLoadTexture(const std::string& filename);

    void deleteTexture(const std::string& filename);
    void deleteTexture(Texture* texture);

private:
    TextureManager();
    ~TextureManager();

private:
    std::map<std::string, Texture*> m_textures;
    bool m_verbose;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_TEXTUREMANAGER_HPP
