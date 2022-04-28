#include <Core/Asset/Image.hpp>
#include <memory>

//#define USE_OIIO

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
    ImageImpl( const ImageSpec& spec, void* data, size_t len ) { m_data = data; }
    ImageImpl( const std::string& filename ) {}
    ~ImageImpl() = default;

    void update( void* newData, size_t len ) { m_data = newData; }
    void resize( int width, int height, void* newData, size_t len ) { m_data = newData; }

  public:
    const void* getData() const { return m_data; }

  private:
    void* m_data;
};

#endif

/////////////////////////////////////////////////////////////////////////
/// CONSTRUCTORS
///
Image::Image( const ImageSpec& spec, void* data, size_t len ) :
    //    m_impl { std::make_unique<ImageImpl>( spec, data ) } {}
    m_impl { new ImageImpl( spec, data, len ) },
    m_spec( spec ),
    m_age { 1 }, // birth
    m_sizeData( spec.width * spec.height * spec.nchannels * g_typeDesc2size.at( spec.format ) ) {

    assert( m_sizeData == len );
}

Image::Image( const std::string& filename ) :
    //    m_impl { std::make_unique<ImageImpl>( filename ) } {}
    m_impl { new ImageImpl( filename ) },
    m_spec( 0, 0, 0 ),
    m_age { 1 } // birth
{
    m_sizeData =
        m_spec.width * m_spec.height * m_spec.nchannels * g_typeDesc2size.at( m_spec.format );
}

Image::~Image() {}

void Image::update( void* newData, size_t len ) {
    assert( len == m_sizeData );
    m_impl->update( newData, len );
    ++m_age;
}

void Image::resize( int width, int height, void* newData, size_t len ) {
    m_spec.width  = width;
    m_spec.height = height;
    m_sizeData =
        m_spec.width * m_spec.height * m_spec.nchannels * g_typeDesc2size.at( m_spec.format );
    assert( len == m_sizeData );
    m_impl->resize( width, height, newData, len );
    ++m_age;
}

const void* Image::getData() const {
    return m_impl->getData();
}

const ImageSpec& Image::get_spec() const {
    return m_spec;
}

size_t Image::getAge() const {
    return m_age;
}

size_t Image::getSizeData() const {
    return m_sizeData;
};

} // namespace Asset
} // namespace Core
} // namespace Ra
