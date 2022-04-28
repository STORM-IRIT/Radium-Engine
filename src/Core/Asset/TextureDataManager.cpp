#include <Core/Asset/TextureDataManager.hpp>

#include <OpenImageIO/imagebuf.h>

namespace Ra {
namespace Core {
namespace Asset {

//std::map<std::string, TextureData> TextureDataManager::s_name2textures;
    std::map<std::string, int> TextureDataManager::s_name2textureId;
    std::vector<TextureData> TextureDataManager::s_textures;

//static void TextureDataManager::addTexture(const std::string &name, const TextureData &texture)
//{
//    assert(s_name2textures.find(name) == s_name2textures.end());
//    s_name2textures[name] = texture;
////    s_name2textures.at(name) = texture;
//}




} // namespace Asset
} // namespace Core
} // namespace Ra
