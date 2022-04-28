#pragma once
#include <string>

//#include <Core/Asset/AssetData.hpp>
#include <Core/Asset/ImageSpec.hpp>
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace Asset {

class ImageImpl;

/** @brief represent image.
 */
// class RA_CORE_API Image : public AssetData
class RA_CORE_API Image
{
  public:
    Image( const ImageSpec& spec, void* data );

    /// @param filename
    /// The image file to read.
    Image( const std::string& filename );

    //    Image (const Image & image) = default;
    //    Image (Image && image) = default;
    //    Image& operator =(const Image & image) = default;
    Image( const Image& image ) = delete;

    ~Image();

    void update( void* newData );
    //    virtual ~Image();
    //    void clear();

    const ImageSpec& get_spec() const;

  protected:
    friend class ImageImpl;
    //    friend class Image;

  private:
    std::unique_ptr<ImageImpl> m_impl; // PIMPL idom
    ImageSpec m_spec;
};

} // namespace Asset
} // namespace Core
} // namespace Ra

#include <Core/Asset/Image.inl>
