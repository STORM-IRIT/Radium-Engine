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
    Image( const ImageSpec& spec, void* data, size_t len );

    /// @param filename
    /// The image file to read.
    Image( const std::string& filename );

    //    Image (Image && image) = default;
    Image( const Image& image )            = delete;
    Image& operator=( const Image& image ) = delete;

    ~Image();

    void update( void* newData, size_t len );
    void resize( int width, int height, void * newData, size_t len);

public:
    const void* getData() const;
    const ImageSpec& get_spec() const;
    size_t getAge() const;
    size_t getSizeData() const;

  protected:
    friend class ImageImpl;

  private:
    std::unique_ptr<ImageImpl> m_impl; // PIMPL idom
    ImageSpec m_spec;
    size_t m_age = 0;
    size_t m_sizeData; // bytes
};

} // namespace Asset
} // namespace Core
} // namespace Ra

#include <Core/Asset/Image.inl>
