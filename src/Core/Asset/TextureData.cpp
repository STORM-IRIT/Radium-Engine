#include <Core/Asset/TextureData.hpp>
#include <memory>

#include <OpenImageIO/imagebuf.h>

namespace Ra {
namespace Core {
namespace Asset {

class TextureImpl
{
  public:
    TextureImpl( const ImageSpec& spec, void* data ) {
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
    TextureImpl( const std::string& filename ) {
        m_imgBuf = OpenImageIO_v2_3::ImageBuf( filename );
    }
    ~TextureImpl() = default;

    void update( void* newData ) {}

  private:
    //    std::string m_filename;
    OpenImageIO_v2_3::ImageBuf m_imgBuf;
};

/////////////////////////////////////////////////////////////////////////
/// CONSTRUCTORS
///
TextureData::TextureData( const ImageSpec& spec, void* data ) :
    //    m_impl { std::make_unique<TextureImpl>( spec, data ) } {}
    m_impl { new TextureImpl( spec, data ) },
    m_spec( spec ) {}

TextureData::TextureData( const std::string& filename ) :
    //    m_impl { std::make_unique<TextureImpl>( filename ) } {}
    m_impl { new TextureImpl( filename ) },
    m_spec( 0, 0, 0 ) {}

// TextureData::~TextureData() {}
TextureData::~TextureData() {}

void TextureData::update( void* newData ) {
    m_impl->update( newData );
}

const ImageSpec &TextureData::get_spec() const
{
    return m_spec;
};

} // namespace Asset
} // namespace Core
} // namespace Ra
