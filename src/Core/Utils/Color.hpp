#ifndef RADIUMENGINE_COLOR_HPP_
#define RADIUMENGINE_COLOR_HPP_

#include <Core/CoreMacros.hpp>
#include <Eigen/Core>
#include <Eigen/Geometry> //homogeneous
#include <random>

namespace Ra {
namespace Core {
namespace Utils {

/**
 * Colors are defined as Vector4, i.e.\ 4 Scalars in RGBA order.
 * Displayable colors should have all their coordinates between 0 and 1.
 */
template <typename _Scalar>
class ColorBase : public Eigen::Matrix<_Scalar, 4, 1> {
  public:
    using VectorType = Eigen::Matrix<_Scalar, 4, 1>;

    inline ColorBase() : ColorBase( _Scalar( 1. ), _Scalar( 1. ), _Scalar( 1. ) ) {}

    inline ColorBase( _Scalar r, _Scalar g, _Scalar b, _Scalar alpha = _Scalar( 1. ) ) :
        VectorType( r, g, b, alpha ) {}

    /**
     * Copy constructor.
     */
    inline ColorBase( const ColorBase<_Scalar>& other ) : VectorType( other ) {}

    /**
     * Copy constructor from Eigen expressions, for example:
     * \code
     *     ColorBase<_Scalar> c( color * 255 );
     * \endcode
     */
    template <typename Derived>
    inline ColorBase( const Eigen::MatrixBase<Derived>& v ) :
        VectorType( v.template cast<_Scalar>() ) {}

    /**
     * Cast operator, mandatory to use Vector arithmetic.
     */
    operator VectorType() { return *this; }

    /**
     * Creates a Color from the given channels.
     */
    template <typename Derived>
    static inline ColorBase fromRGB( const Eigen::MatrixBase<Derived>& rgb,
                                     Scalar alpha = Scalar( 1. ) ) {
        ColorBase c( rgb.template cast<_Scalar>().homogeneous() );
        c.alpha() = alpha;
        return c;
    }

    /**
     * Return the red, green and blue channels of the Color.
     */
    Eigen::Block<VectorType, 3, 1> rgb() { return ( *this ).template head<3>(); }

    /**
     * Return the red, green and blue channels of the Color.
     */
    const Eigen::Block<VectorType, 3, 1> rgb() const { return ( *this ).template head<3>(); }

    /**
     * Return the alpha channel of the Color.
     */
    Scalar alpha() const { return ( *this )( 3 ); }

    /**
     * Return the alpha channel of the Color.
     */
    Scalar& alpha() { return ( *this )( 3 ); }

    /// \name Predifined Colors
    /// \{

    /**
     * Creates a pure alpha Color, i.e.\ black with alpha == 0.
     */
    static inline ColorBase<_Scalar> Alpha() {
        return ColorBase<_Scalar>( _Scalar( 0. ), _Scalar( 0. ), _Scalar( 0. ), _Scalar( 0. ) );
    }

    /**
     * Creates a black Color.
     */
    static inline ColorBase<_Scalar> Black() {
        return ColorBase<_Scalar>( _Scalar( 0. ), _Scalar( 0. ), _Scalar( 0. ) );
    }

    /**
     * Creates a red Color.
     */
    static inline ColorBase<_Scalar> Red() {
        return ColorBase<_Scalar>( _Scalar( 1. ), _Scalar( 0. ), _Scalar( 0. ) );
    }

    /**
     * Creates a green Color.
     */
    static inline ColorBase<_Scalar> Green() {
        return ColorBase<_Scalar>( _Scalar( 0. ), _Scalar( 1. ), _Scalar( 0. ) );
    }

    /**
     * Creates a blue Color.
     */
    static inline ColorBase<_Scalar> Blue() {
        return ColorBase<_Scalar>( _Scalar( 0. ), _Scalar( 0. ), _Scalar( 1. ) );
    }

    /**
     * Creates a black Color.
     */
    static inline ColorBase<_Scalar> Yellow() {
        return ColorBase<_Scalar>( _Scalar( 1. ), _Scalar( 1. ), _Scalar( 0. ) );
    }

    /**
     * Creates a magenta Color.
     */
    static inline ColorBase<_Scalar> Magenta() {
        return ColorBase<_Scalar>( _Scalar( 1. ), _Scalar( 0. ), _Scalar( 1. ) );
    }

    /**
     * Creates a cyan Color.
     */
    static inline ColorBase<_Scalar> Cyan() {
        return ColorBase<_Scalar>( _Scalar( 0. ), _Scalar( 1. ), _Scalar( 1. ) );
    }

    /**
     * Creates a white Color.
     */
    static inline ColorBase<_Scalar> White() {
        return ColorBase<_Scalar>( _Scalar( 1. ), _Scalar( 1. ), _Scalar( 1. ) );
    }

    /**
     * Creates an \p f -grey Color, with \p a alpha.
     */
    static inline ColorBase<_Scalar> Grey( _Scalar f = _Scalar( 0.5 ), _Scalar a = _Scalar( 1. ) ) {
        return ColorBase<_Scalar>( f, f, f, a );
    }

    /**
     * Creates a "skin" Color.
     */
    static inline ColorBase<_Scalar> Skin() {
        return ColorBase<_Scalar>( _Scalar( 1.0 ), _Scalar( 0.87 ), _Scalar( 0.74 ) );
    }
    /// \}

    /// \name Convertions to/from various int and color formats
    /// \{

    /**
     * Creates a Color from the 4 given 1-byte channels.
     */
    static inline ColorBase<_Scalar> fromChars( uchar r, uchar g, uchar b, uchar a = 0xff ) {
        return ColorBase<_Scalar>( _Scalar( r ) / 255.0f, _Scalar( g ) / 255.0f,
                                   _Scalar( b ) / 255.0f, _Scalar( a ) / 255.0f );
    }

    /**
     * Creates a Color from the given 4-bytes packed channels.
     * \note Channels are expected to be packed in rgba order.
     */
    static inline ColorBase<_Scalar> fromRGBA32( uint32_t rgba ) {
        uchar r = uchar( ( rgba >> 24 ) & 0xff );
        uchar g = uchar( ( rgba >> 16 ) & 0xff );
        uchar b = uchar( ( rgba >> 8 ) & 0xff );
        uchar a = uchar( ( rgba >> 0 ) & 0xff );
        return fromChars( r, g, b, a );
    }

    /**
     * Creates a Color from the given 4-bytes packed channels.
     * \note Channels are expected to be packed in argb order.
     */
    static inline ColorBase<_Scalar> fromARGB32( uint32_t argb ) {
        uchar a = uchar( ( argb >> 24 ) & 0xff );
        uchar r = uchar( ( argb >> 16 ) & 0xff );
        uchar g = uchar( ( argb >> 8 ) & 0xff );
        uchar b = uchar( ( argb >> 0 ) & 0xff );
        return fromChars( r, g, b, a );
    }

    /**
     * Creates a Color from the HSVA channels.
     */
    static inline ColorBase<_Scalar> fromHSV( const _Scalar hue, const _Scalar saturation = 1.0,
                                              const _Scalar value = 1.0,
                                              const _Scalar alpha = 1.0 ) {
        ColorBase<_Scalar> c;

        if ( saturation == 0.0f )
        {
            c[0] = c[1] = c[2] = value;
            c[3] = alpha;
            return c;
        }
        _Scalar h = ( ( hue == 1.0f ) ? 0.0f : hue ) * 6.0f;
        int i = int( std::floor( h ) );
        _Scalar v1 = value * ( 1.0f - saturation );
        _Scalar v2 = value * ( 1.0f - ( saturation * ( h - i ) ) );
        _Scalar v3 = value * ( 1.0f - ( saturation * ( 1.0f - h - i ) ) );
        switch ( i )
        {
        case 0:
        {
            c[0] = value;
            c[1] = v3;
            c[2] = v1;
        }
        break;
        case 1:
        {
            c[0] = v2;
            c[1] = value;
            c[2] = v1;
        }
        break;
        case 2:
        {
            c[0] = v1;
            c[1] = value;
            c[2] = v3;
        }
        break;
        case 3:
        {
            c[0] = v1;
            c[1] = v2;
            c[2] = value;
        }
        break;
        case 4:
        {
            c[0] = v3;
            c[1] = v1;
            c[2] = value;
        }
        break;
        default:
        {
            c[0] = value;
            c[1] = v1;
            c[2] = v2;
        }
        break;
        }
        c[3] = alpha;
        return c;
    }

    /**
     * Pack the Color channels into a 4-bytes packed channels, in rgba order.
     */
    inline uint32_t toRGBA32() const {
        ColorBase<_Scalar> c( ( *this ) * 255 );
        Eigen::Matrix<int, 4, 1> scaled( c.x(), c.y(), c.z(), c.w() );
        return ( uint32_t( scaled( 0 ) ) << 24 ) | ( uint32_t( scaled( 1 ) ) << 16 ) |
               ( uint32_t( scaled( 2 ) ) << 8 ) | ( uint32_t( scaled( 3 ) ) << 0 );
    }

    /**
     * Pack the Color channels into a 4-bytes packed channels, in argb order.
     */
    inline uint32_t toARGB32() const {
        ColorBase<_Scalar> c( ( *this ) * 255 );
        Eigen::Matrix<int, 4, 1> scaled( c.x(), c.y(), c.z(), c.w() );
        return ( uint32_t( scaled( 3 ) ) << 24 ) | ( uint32_t( scaled( 0 ) ) << 16 ) |
               ( uint32_t( scaled( 1 ) ) << 8 ) | ( uint32_t( scaled( 2 ) ) << 0 );
    }
    /// \}

    /**
     * Creates \p size Colors scatterring the HSV space with gamma correction \p gamma.
     * The Colors are then randomly shuffled.
     */
    static inline std::vector<ColorBase<_Scalar>> scatter( const uint size, const _Scalar gamma ) {
        std::vector<ColorBase<_Scalar>> color( size );
        if ( size > 1 )
            for ( uint i = 0; i < size; ++i )
            {
                color[i] = fromHSV( ( _Scalar( i ) / _Scalar( size - 1 ) ) * 0.777 );
                color[i] = ( color[i] + ColorBase<_Scalar>::Constant( gamma ) ) * 0.5;
            }
        else
        { color[0] = Red(); }
        std::shuffle( color.begin(), color.end(), std::mt19937( std::random_device()() ) );
        return color;
    }
};

using Color = ColorBase<Scalar>;
using Colorf = ColorBase<float>;
using Colord = ColorBase<double>;

} // namespace Utils
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_COLOR_PRESET_HPP_
