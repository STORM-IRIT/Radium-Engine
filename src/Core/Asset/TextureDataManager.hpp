#pragma once
#include <string>
#include <vector>

#include <Core/Asset/TextureData.hpp>
#include <Core/RaCore.hpp>
#include <map>

namespace Ra {
namespace Core {
namespace Asset {

/** @brief represent texture.
 */
class RA_CORE_API TextureDataManager
{
  public:
    static void addTexture( const std::string& name, TextureData& texture ) {
        assert( s_name2textureId.find( name ) == s_name2textureId.end() );
//        m_textures.push_back( texture );
        //        s_name2textureId
//        s_name2textureId.emplace( name, texture );

        //        s_name2textureId.insert(name, texture );
        //        s_name2textureId.insert(std::make_pair<std::string, TextureData>( name, texture ));
    }

    static TextureData& getTexture( const std::string& name ) {
        assert( s_name2textureId.find( name ) != s_name2textureId.end() );
        return s_textures[s_name2textureId.at( name )];
    }

    //    void clear() {
    //        for ( auto& pair : s_name2textureId ) {
    //            auto& texture = pair.second;
    //            texture.clear();
    //        }
    //        s_name2textureId.clear();
    //    }

  private:
    //    static std::unordered_map<std::string, TextureData> s_name2textures;
    static std::map<std::string, int> s_name2textureId;
    static std::vector<TextureData> s_textures;
};

} // namespace Asset
} // namespace Core
} // namespace Ra

#include <Core/Asset/TextureDataManager.inl>
