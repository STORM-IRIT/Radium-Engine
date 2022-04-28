#pragma once
#include <string>
#include <vector>

//#include <Core/Asset/AssetData.hpp>
#include <Core/Asset/ImageSpec.hpp>
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace Asset {

class TextureImpl;

/** @brief represent texture.
 */
//class RA_CORE_API TextureData : public AssetData
class RA_CORE_API TextureData
{
  public:
    TextureData( const ImageSpec& spec, void* data );

    //    TextureData (const TextureData & texture) = default;
    //    TextureData (TextureData && texture) = default;
    //    TextureData& operator =(const TextureData & texture) = default;

    /// @param filename
    /// The image file to read.
    TextureData( const std::string& filename );

    ~TextureData();

    void update(void* newData);
    //    virtual ~TextureData();
//    void clear();

    const ImageSpec &get_spec() const;

protected:
    friend class TextureImpl;
    //    friend class TextureData;

  private:
    std::unique_ptr<TextureImpl> m_impl; // PIMPL idom
    ImageSpec m_spec;
};

} // namespace Asset
} // namespace Core
} // namespace Ra

#include <Core/Asset/TextureData.inl>
