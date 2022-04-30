#include <Core/Asset/Image.hpp>
#include <memory>

//#define USE_OIIO
#include <iostream>
//#include <string>
#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <filesystem>

namespace Ra {
namespace Core {
namespace Asset {

#ifdef USE_OIIO
#    include <OpenImageIO/imagebuf.h>
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

class ImageImpl
{
  public:
    ImageImpl( const ImageSpec& spec, void* data, size_t len ) // : m_spec( spec )
    {
        m_sizeData = len;
        m_data     = new unsigned char[m_sizeData];
        update( data, m_sizeData );
    }
    ImageImpl( const std::string& filename ) {
        assert( std::filesystem::exists( filename ) );
        int width;
        int height;
        int nChannel;
        int desired_channels = 3;
        unsigned char* data =
            stbi_load( filename.c_str(), &width, &height, &nChannel, desired_channels );

        if ( !data ) {
            std::cout << "Something went wrong when loading image \"" << filename << "\".";
            return;
        }

        m_sizeData = width * height * nChannel;
        m_data     = new unsigned char[m_sizeData];
        update( data, m_sizeData );
    }
    ~ImageImpl() = default;

    void update( void* newData, size_t len ) {
        // no update if newData is null
        // newData can be null when user want to create an image without having the data
        // only spec was specify and the data comming after the instanciate of image (from sensors
        // for example)
        if ( newData == nullptr ) return;

        assert( m_data != nullptr );
        assert( m_sizeData == len );
        //        m_data = newData;
        std::memcpy( m_data, newData, len );
        // copy the data here, no dangling pointers,
        // we assume that the client pointer can be deallocated by himself
    }
    void resize( int width, int height, void* newData, size_t len ) {
        if ( m_sizeData != len ) {
            delete[]( unsigned char* ) m_data;
            m_sizeData = len;
            m_data     = new unsigned char[m_sizeData];
        }
        update( newData, len );
    }

  public:
    const void* getData() const { return m_data; }
    //    const ImageSpec& getSpec() const { return m_spec; }
    size_t getSizeData() const { return m_sizeData; }

  private:
    //    void setData(void * data, size_t len);
  private:
    void* m_data = nullptr;
    //    unsigned char* m_data = nullptr;
    //    ImageSpec m_spec;
    size_t m_sizeData = 0; // bytes
};

#endif

/////////////////////////////////////////////////////////////////////////
/// CONSTRUCTORS
///
Image::Image( const ImageSpec& spec, void* data, size_t len ) :
    //    m_impl { std::make_unique<ImageImpl>( spec, data ) } {}
    m_impl { new ImageImpl( spec, data, len ) },
    m_age { 1 } // birth
{}

Image::Image( const std::string& filename ) :
    //    m_impl { std::make_unique<ImageImpl>( filename ) } {}
    m_impl { new ImageImpl( filename ) },
    m_age { 1 } // birth
{
    //    m_sizeData =
    //        m_spec.width * m_spec.height * m_spec.nchannels * g_typeDesc2size.at( m_spec.format );
}

Image::~Image() {}

void Image::update( void* newData, size_t len ) {
    m_impl->update( newData, len );
    ++m_age;
}

void Image::resize( int width, int height, void* newData, size_t len ) {
    //    m_spec.width  = width;
    //    m_spec.height = height;
    //    m_sizeData =
    //        m_spec.width * m_spec.height * m_spec.nchannels * g_typeDesc2size.at( m_spec.format );
    //    assert( m_sizeData == len );
    m_impl->resize( width, height, newData, len );
    //    m_impl->setData (newData, len);
    ++m_age;
}

// void Image::setData(void *data, size_t len)
//{

//};
size_t Image::getAge() const {
    return m_age;
}

const void* Image::getData() const {
    return m_impl->getData();
}

// const ImageSpec& Image::get_spec() const {
//     return m_impl->getSpec();
// }

size_t Image::getSizeData() const {
    return m_impl->getSizeData();
}

} // namespace Asset
} // namespace Core
} // namespace Ra
