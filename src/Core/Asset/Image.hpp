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
    Image( ImageSpec spec, void* data, size_t len );

    /// @param filename
    /// The image file to read.
    explicit Image( const std::string& filename );

    Image( const Image& image ) = delete;
    Image& operator=( const Image& image ) = delete;

    ~Image();

    void update( void* newData, size_t len );
    void resize( int width, int height, void* newData, size_t len );

  protected:
    friend class ImageImpl;

  private:
    std::unique_ptr<ImageImpl> m_impl; // PIMPL idom
    size_t m_age = 0;

  public:
    const void* getData() const;
    size_t getAge() const;
    size_t getSizeData() const;
    size_t getWidth() const;
    size_t getHeight() const;
    size_t getNChannels() const;
};

} // namespace Asset
} // namespace Core
} // namespace Ra

#include <Core/Asset/Image.inl>
