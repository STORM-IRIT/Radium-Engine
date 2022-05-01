#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <Core/Asset/Image.hpp>
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace Asset {

/** @brief represent image.
 */
class RA_CORE_API ImageManager
{
  public:
    static void addTexture( const std::string& name, std::shared_ptr<Image> image ) {
        assert( s_name2image.find( name ) == s_name2image.end() );
        s_name2image[name] = image;
    }

    static std::shared_ptr<Image> getTexture( const std::string& name ) {
        assert( s_name2image.find( name ) != s_name2image.end() );
        return s_name2image.at( name );
    }

    //    void clear() {
    //        for ( auto& pair : s_name2image ) {
    //            auto& image = pair.second;
    //            image.clear();
    //        }
    //        s_name2image.clear();
    //    }

  private:
    static std::map<std::string, std::shared_ptr<Ra::Core::Asset::Image>> s_name2image;
};

} // namespace Asset
} // namespace Core
} // namespace Ra

#include <Core/Asset/ImageManager.inl>
