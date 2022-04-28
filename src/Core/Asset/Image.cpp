#include <Core/Asset/Image.hpp>
#include <memory>

#include <OpenImageIO/imagebuf.h>

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
    ImageImpl( const std::string& filename ) {
        m_imgBuf = OpenImageIO_v2_3::ImageBuf( filename );
    }
    ~ImageImpl() = default;

    void update( void* newData ) {}

  private:
    //    std::string m_filename;
    OpenImageIO_v2_3::ImageBuf m_imgBuf;
};

/////////////////////////////////////////////////////////////////////////
/// CONSTRUCTORS
///
Image::Image( const ImageSpec& spec, void* data ) :
    //    m_impl { std::make_unique<ImageImpl>( spec, data ) } {}
    m_impl { new ImageImpl( spec, data ) },
    m_spec( spec ) {}

Image::Image( const std::string& filename ) :
    //    m_impl { std::make_unique<ImageImpl>( filename ) } {}
    m_impl { new ImageImpl( filename ) },
    m_spec( 0, 0, 0 ) {}

// Image::~Image() {}
Image::~Image() {}

void Image::update( void* newData ) {
    m_impl->update( newData );
}

const ImageSpec &Image::get_spec() const
{
    return m_spec;
};

} // namespace Asset
} // namespace Core
} // namespace Ra
