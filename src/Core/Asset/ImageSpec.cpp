#include <Core/Asset/ImageSpec.hpp>

namespace Ra {
namespace Core {
namespace Asset {

void ImageSpec::default_channel_names() noexcept {
    channelnames.clear();
    channelnames.reserve( nchannels );
    alpha_channel = -1;
    z_channel     = -1;
    if ( nchannels == 1 ) { // Special case: 1-channel is named "Y"
        channelnames.emplace_back( "Y" );
        return;
    }
    // General case: name channels R, G, B, A, channel4, channel5, ...
    if ( nchannels >= 1 ) channelnames.emplace_back( "R" );
    if ( nchannels >= 2 ) channelnames.emplace_back( "G" );
    if ( nchannels >= 3 ) channelnames.emplace_back( "B" );
    if ( nchannels >= 4 ) {
        channelnames.emplace_back( "A" );
        alpha_channel = 3;
    }
    //    for (int c = 4; c < nchannels; ++c)
    //        channelnames.push_back(Strutil::fmt::format("channel{}", c));
}

ImageSpec::ImageSpec( int xres, int yres, int nchans, TypeDesc format ) noexcept :
    x( 0 ),
    y( 0 ),
    z( 0 ),
    width( xres ),
    height( yres ),
    depth( 1 ),
    full_x( 0 ),
    full_y( 0 ),
    full_z( 0 ),
    full_width( xres ),
    full_height( yres ),
    full_depth( 1 ),
    tile_width( 0 ),
    tile_height( 0 ),
    tile_depth( 1 ),
    nchannels( nchans ),
    format( format ),
    alpha_channel( -1 ),
    z_channel( -1 ),
    deep( false ) {
    default_channel_names();
}

} // namespace Asset
} // namespace Core
} // namespace Ra
