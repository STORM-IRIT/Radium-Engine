#include <Core/Asset/Image.hpp>

//#define USE_OIIO

#ifdef USE_OIIO

#    include <OpenImageIO/imagebuf.h>
namespace Ra {
namespace Core {
namespace Asset {
class ImageImpl
{
  public:
    ImageImpl( const ImageSpec& spec, void* data ) {
        OpenImageIO_v2_3::ImageSpec imgSpec;
        //        assert( sizeof( spec ) == sizeof( imgSpec ) );

        assert( sizeof( spec.format ) == sizeof( imgSpec.format ) );
        memcpy( &imgSpec, &spec, (uintptr_t)&spec.channelnames - (uintptr_t)&spec );

        imgSpec.channelnames = spec.channelnames;
        memcpy( &imgSpec.alpha_channel,
                &spec.alpha_channel,
                (uintptr_t)&spec.deep - (uintptr_t)&spec.alpha_channel + sizeof( spec.deep ) );
        //        for ( const auto& extra_attrib : spec.extra_attribs ) {
        //            OpenImageIO_v2_3::ParamValue paramValue;
        ////            memcpy( &paramValue, &extra_attrib, sizeof( paramValue ) );
        //            imgSpec.extra_attribs.push_back( paramValue );
        //        }
        //        imgSpec.extra_attribs = spec.extra_attribs;
        m_imgBuf = OpenImageIO_v2_3::ImageBuf( imgSpec, data );
        assert( (void*)m_imgBuf.pixeladdr( 0, 0, 0 ) == data );
    }
    ImageImpl( const std::string& filename ) { m_imgBuf = OpenImageIO_v2_3::ImageBuf( filename ); }
    ~ImageImpl() = default;

    void update( void* newData ) {
        m_imgBuf = OpenImageIO_v2_3::ImageBuf( m_imgBuf.spec(), newData );
        assert( (void*)m_imgBuf.pixeladdr( 0, 0, 0 ) == newData );
    }
    const void* getData() const { return m_imgBuf.pixeladdr( 0, 0, 0 ); }

  private:
    //    std::string m_filename;
    OpenImageIO_v2_3::ImageBuf m_imgBuf;
};

#else

#    include <memory>

#    include <cstring>
#    include <iostream>

#    define STB_IMAGE_IMPLEMENTATION
#    include <stb/stb_image.h>

#    include <filesystem>
#include <mutex>
#include <atomic>

namespace Ra {
namespace Core {
namespace Asset {

class ImageImpl
{
  public:
    ImageImpl( ImageSpec&& spec, void* data, size_t len ) : m_spec( std::move( spec ) ) {

        assert( len == m_spec.width * m_spec.height * m_spec.nchannels );
        m_sizeData = len;
        //        m_data     = new unsigned char[m_sizeData];
        auto* readWriteBuffs = new unsigned char[m_sizeData * 2]; // do this to benefit of cache
        m_data[0]            = readWriteBuffs;
        m_data[1]            = &readWriteBuffs[m_sizeData];
        update( data, m_sizeData );
    }

    ImageImpl( const std::string& filename ) : m_spec( 0, 0, 0, TypeUInt8 ) {

        assert( std::filesystem::exists( filename ) );
        int desired_channels = 0;
        unsigned char* data  = stbi_load(
            filename.c_str(), &m_spec.width, &m_spec.height, &m_spec.nchannels, desired_channels );

        if ( !data ) {
            std::cout << "Something went wrong when loading image \"" << filename << "\".";
            return;
        }

        m_sizeData = m_spec.width * m_spec.height * m_spec.nchannels;
        // no need to copy data, we are the owner of the image pointer
        //        m_data = new unsigned char[m_sizeData];
        //        update( data, m_sizeData );
        m_data[m_readHead] = data;
    }

    ~ImageImpl() {
        if ( m_data[0] != nullptr ) {
            delete[]( unsigned char* )  m_data[0];
            m_data[0] = nullptr;
            m_data[1] = nullptr;
        }
    }

    void update( void* newData, size_t len ) {
        if ( m_data[1] == nullptr ) return; // no update possible with from file image

        // no update if newData is null
        // newData can be null when user want to create an image without having the data
        // only spec was specify and the data comming after the instanciate of image (from
        // sensors for example)
        if ( newData == nullptr ) return;

        assert( m_data[0] != nullptr && m_data[1] != nullptr );
        if (m_sizeData != len) {
            std::cout << "hello" << std::endl;
        }
        assert( m_sizeData == len );

        // copy user data here, no dangling pointers,
        // we assume that the client pointer can be deallocated by himself
        // the client data can be modified at any time
        // with a client pointer it's difficult to manage the read/write operations
        // on the image buffer without asking the clients to manage the concurrency (mutex,
        // semaphore)
        const int writeHead = 1 - m_readHead;
            m_mtx.lock();
        std::memcpy( m_data[writeHead], newData, len );
        m_mtx.unlock();
        m_readHead = writeHead;
        //        m_data = newData; // avoid this
    }

    void resize( int width, int height, void* newData, size_t len ) {
        if ( m_sizeData != len ) {
            m_spec.width  = width;
            m_spec.height = height;

            assert( len == m_spec.width * m_spec.height * m_spec.nchannels );
            m_sizeData = len;
            m_mtx.lock();
            delete[]( unsigned char* ) m_data[0]; // I don't know if both array are deallocate
            auto* data = new unsigned char[m_sizeData * 2]; // do this to benefit of cache
            m_data[0]  = data;
            m_data[1]  = &data[m_sizeData];
            m_mtx.unlock();
        }
        update( newData, len );
    }

  public:
    const void* getData() const { return m_data[m_readHead]; }
    size_t getSizeData() const { return m_sizeData; }
    size_t getWidth() const { return m_spec.width; }
    size_t getHeight() const { return m_spec.height; }
    size_t getNChannels() const { return m_spec.nchannels; }

  private:
    void* m_data[2] = { nullptr, nullptr }; // read/write buffers
    std::mutex m_mtx;
    std::atomic<int> m_readHead = 0;
    ImageSpec m_spec;
    size_t m_sizeData = 0; // bytes
};

#endif

//////////////////////////// Image /////////////////////////////////////////////

Image::Image( ImageSpec spec, void* data, size_t len ) :
    m_impl { new ImageImpl( std::move( spec ), data, len ) },
    m_age { 1 } // birth
{}

Image::Image( const std::string& filename ) :
    m_impl { new ImageImpl( filename ) },
    m_age { 1 } // birth
{}

Image::~Image() {}

void Image::update( void* newData, size_t len ) {
    m_impl->update( newData, len );
    ++m_age;
}

void Image::resize( int width, int height, void* newData, size_t len ) {
    m_impl->resize( width, height, newData, len );
    ++m_age;
}

size_t Image::getAge() const {
    return m_age;
}

const void* Image::getData() const {
    return m_impl->getData();
}

size_t Image::getSizeData() const {
    return m_impl->getSizeData();
}

size_t Image::getWidth() const {
    return m_impl->getWidth();
}

size_t Image::getHeight() const {
    return m_impl->getHeight();
}

size_t Image::getNChannels() const {
    return m_impl->getNChannels();
}

} // namespace Asset
} // namespace Core
} // namespace Ra
