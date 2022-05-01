#include <Core/Asset/Image.hpp>

#undef USE_OIIO // not ready
#ifdef USE_OIIO

#    include <OpenImageIO/imagebuf.h>

namespace Ra {
namespace Core {
namespace Asset {

// todo, this implement not worked today

class ImageImpl
{
  public:
    ImageImpl( ImageSpec&& spec, void* data, size_t len ) {
        OpenImageIO_v2_3::ImageSpec imgSpec;

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
        m_sizeData = len;
    }

    ImageImpl( const std::string& filename ) {
        m_imgBuf = OpenImageIO_v2_3::ImageBuf( filename );
        // todo init m_sizeData
        m_sizeData = m_imgBuf.nativespec().size_t_safe();
    }

    ~ImageImpl() { m_imgBuf.clear(); }

    void update( void* newData, size_t len ) {
        m_imgBuf = OpenImageIO_v2_3::ImageBuf( m_imgBuf.spec(), newData );
        assert( (void*)m_imgBuf.pixeladdr( 0, 0, 0 ) == newData );
        m_sizeData = len;
    }

    void resize( int width, int height, void* newData, size_t len ) {
        auto imgSpec   = m_imgBuf.spec();
        imgSpec.width  = width;
        imgSpec.height = height;
        m_imgBuf       = OpenImageIO_v2_3::ImageBuf( imgSpec, newData );
        assert( (void*)m_imgBuf.pixeladdr( 0, 0, 0 ) == newData );
        m_sizeData = len;
    }

  public:
    const void* getData() const { return m_imgBuf.pixeladdr( 0, 0, 0 ); }
    size_t getSizeData() const { return m_sizeData; }
    size_t getWidth() const { return m_imgBuf.spec().width; }
    size_t getHeight() const { return m_imgBuf.spec().height; }
    size_t getNChannels() const { return m_imgBuf.spec().nchannels; }

  private:
    //    std::string m_filename;
    OpenImageIO_v2_3::ImageBuf m_imgBuf;
    size_t m_sizeData = 0;
};

#else

#    include <memory>

#    include <cstring>
#    include <iostream>

#    define STB_IMAGE_IMPLEMENTATION
#    include <stb/stb_image.h>

#    include <filesystem>

namespace Ra {
namespace Core {
namespace Asset {

class ImageImpl
{
  public:
    ImageImpl( ImageSpec&& spec, void* data, size_t len ) {
        m_width     = spec.width;
        m_height    = spec.height;
        m_nChannels = spec.nchannels;

        assert( len == m_width * m_height * m_nChannels );
        m_sizeData           = len;
        auto* readWriteBuffs = new unsigned char[m_sizeData * 2]; // do this to benefit of cache
        m_data[0]            = readWriteBuffs;
        m_data[1]            = &readWriteBuffs[m_sizeData];
        update( data, m_sizeData );
    }

    ImageImpl( const std::string& filename ) {
        assert( std::filesystem::exists( filename ) );
        int desired_channels = 0;
        unsigned char* data  = stbi_load( filename.c_str(),
                                         (int*)&m_width,
                                         (int*)&m_height,
                                         (int*)&m_nChannels,
                                         desired_channels );

        if ( !data ) {
            std::cout << "Something went wrong when loading image \"" << filename << "\"."
                      << std::endl;
            return;
        }

        m_sizeData = m_width * m_height * m_nChannels;
        // no need to copy data, we are the owner of the image pointer
        //        m_data = new unsigned char[m_sizeData];
        //        update( data, m_sizeData );
        m_data[m_readHead] = data;
    }

    ~ImageImpl() {
        if ( m_data[0] != nullptr ) {
            delete[]( unsigned char* ) m_data[0];
            m_data[0] = nullptr;
            m_data[1] = nullptr;
        }
    }

    void update( void* newData, size_t len ) {
        if ( m_data[1] == nullptr ) return; // no update possible with from file image

        // no update if newData is null
        // newData can be null when user want to create an image without having the data
        // only image specification was specify and the data comming after the instanciate of image
        // (comming afterward from sensors for example)
        if ( newData == nullptr ) return;

        assert( m_data[0] != nullptr && m_data[1] != nullptr );
        assert( m_sizeData == len );

        // copy user data here, no dangling pointers,
        // we assume that the client pointer can be deallocated by himself
        // the client data can be modified at any time (network socket, stream sensors)
        // with a client pointer it's difficult to manage the read/write operations
        // on the image buffer without asking the clients to manage the concurrency
        // (mutex, semaphore)
        const int writeHead = 1 - m_readHead;
        std::memcpy( m_data[writeHead], newData, len );
        m_readHead = writeHead;
    }

    void resize( int width, int height, void* newData, size_t len ) {
        // I assume here that there is only one thread that modifies or redefines an image
        // so there can't be 2 concurrent calls of resize and update (so no mutex here)
        assert( m_sizeData != len );
        assert( newData != nullptr );

        struct {
            void* data[2];
            size_t width;
            size_t height;
            size_t sizeData;
            int readHead;
        } tmp;

        tmp.width  = width;
        tmp.height = height;

        assert( len == tmp.width * tmp.height * m_nChannels );
        tmp.sizeData = len;

        auto* readWriteBuffs = new unsigned char[tmp.sizeData * 2]; // do this to benefit of cache
        tmp.data[0]          = readWriteBuffs;
        tmp.data[1]          = &readWriteBuffs[tmp.sizeData];

        const int writeHead = 1 - m_readHead;
        std::memcpy( tmp.data[writeHead], newData, len );
        tmp.readHead = writeHead;

        void* dataToRemove = m_data[0];
        memcpy( this, &tmp, sizeof( tmp ) );
        delete[]( unsigned char* ) dataToRemove; // I don't know if both array are deallocate
    }

  public:
    const void* getData() const { return m_data[m_readHead]; }
    size_t getSizeData() const { return m_sizeData; }
    size_t getWidth() const { return m_width; }
    size_t getHeight() const { return m_height; }
    size_t getNChannels() const { return m_nChannels; }

  private:
    void* m_data[2] = { nullptr, nullptr }; // read/write buffers

    size_t m_width    = 0;
    size_t m_height   = 0;
    size_t m_sizeData = 0; // bytes
    int m_readHead    = 0;

    size_t m_nChannels = 0;
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
