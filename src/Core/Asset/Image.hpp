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
    /**
     * @brief Image
     * Construct a writable Image that copy existing pixel memory
     * owned by the calling application.
     * @param spec
     * Image specification.
     * @param data
     * User data to copy into image.
     * @param len
     * Size of copy in bytes.
     */
    Image( ImageSpec spec, const void* data, size_t len );

    /**
     * @brief Image
     * Construct a writable Image that “wraps” existing pixel memory
     * owned by the calling application. The Image does not own the pixel storage
     * and will will not free/delete that memory, even when the Image is destroyed.
     * @param spec
     * Image specification.
     * @param data
     * User image pointer data to wrap.
     */
    Image( ImageSpec spec, void* data );

    /**
     * @brief Image
     * Construct a read-only Image that will be used to read the named file
     * @param filename
     * Image file path on system.
     */
    explicit Image( const std::string& filename );

    Image( const Image& image ) = delete;
    Image& operator=( const Image& image ) = delete;

    ~Image();

    /**
     * @brief update
     * Update internal copy of user pixel image pointer.
     * @param data
     * Copy from this pointer.
     * @param len
     * Size of copy.
     */
    void update( const void* data, size_t len );

    /**
     * @brief update
     * Update external user pixel image pointer.
     * @param data
     * User image pointer.
     */
    void update( void* data );

    /**
     * @brief resize
     * Resize image and copy from external image pointer.
     * @param width
     * @param height
     * @param data
     * @param len
     */
    void resize( int width, int height, const void* data, size_t len );

    /**
     * @brief resize
     * Resize image and set new external image pointer.
     * @param width
     * @param height
     * @param data
     */
    void resize( int width, int height, void* data );

  protected:
    friend class ImageImpl;

  private:
    std::unique_ptr<ImageImpl> m_impl; // PIMPL idom
    size_t m_age = 0;

  public: // getters
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
