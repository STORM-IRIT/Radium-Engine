#ifndef RADIUMENGINE_COLOR_PRESET_HPP_
#define RADIUMENGINE_COLOR_PRESET_HPP_

#include <Core/Math/LinearAlgebra.hpp>

namespace Ra{
    namespace Core{

        /// Colors are defined as vector4, i.e. 4 floats in RGBA order.
        /// displayable colors should have all their coordinates between 0 and 1.
        namespace Colors
        {
            // Primary and secondary colors.
            // TODO (Val) : check if we can make these constexpr
            inline Color Alpha() { return Color( 0.0, 0.0, 0.0, 0.0 ); }
            inline Color Black() { return Color(0,0,0,1); }

            inline Color Red()   { return Color(1,0,0,1); }
            inline Color Green() { return Color(0,1,0,1); }
            inline Color Blue()  { return Color(0,0,1,1); }

            inline Color Yellow()  { return Color(1,1,0,1); }
            inline Color Magenta() { return Color(1,0,1,1); }
            inline Color Cyan()    { return Color(0,1,1,1); }

            inline Color White()   { return Color(1,1,1,1); }

            inline Color Grey( Scalar f = 0.5f) { return Color(f,f,f,1);}

            // Convert to/from various int formats
            inline Color FromChars(uchar r, uchar g, uchar b, uchar a = 0xff)
            {
                return Color(Scalar(r)/255.0f, Scalar(g)/255.0f, Scalar(b)/255.0f, Scalar(a)/255.0f );
            }

            inline Color FromRGBA32(uint32_t rgba)
            {
                uchar r = uchar((rgba >> 24) & 0xff);
                uchar g = uchar((rgba >> 16) & 0xff);
                uchar b = uchar((rgba >>  8) & 0xff);
                uchar a = uchar((rgba >>  0) & 0xff);
                return FromChars(r,g,b,a);
            }

            inline Color fromHSV( const Scalar hue,
                                  const Scalar saturation = 1.0,
                                  const Scalar value      = 1.0,
                                  const Scalar alpha      = 1.0 ) {
                Color c;
                if( saturation == 0.0f ) {
                    c[0] = c[1] = c[2] = value;
                    c[3] = alpha;
                    return c;
                }
                Scalar h  = ( ( hue == 1.0f ) ? 0.0f : hue ) * 6.0f;
                int    i  = std::floor( h );
                Scalar v1 = value * ( 1.0f - saturation );
                Scalar v2 = value * ( 1.0f - ( saturation * ( h - i ) ) );
                Scalar v3 = value * ( 1.0f - ( saturation * ( 1.0f - h - i ) ) );
                switch( i ) {
                    case 0: {
                        c[0] = value; c[1] = v3;    c[2] = v1;
                    } break;
                    case 1: {
                        c[0] = v2;    c[1] = value; c[2] = v1;
                    } break;
                    case 2: {
                        c[0] = v1;    c[1] = value; c[2] = v3;
                    } break;
                    case 3: {
                        c[0] = v1;    c[1] = v2;    c[2] = value;
                    } break;
                    case 4: {
                        c[0] = v3;    c[1] = v1;    c[2] = value;
                    } break;
                    default: {
                        c[0] = value; c[1] = v1;    c[2] = v2;
                    } break;
                }
                c[3] = alpha;
                return c;
            }

            inline uint32_t ToRGBA32(const Color& color)
            {
                Vector4i scaled = (color * 255).cast<int>();
                return (uint32_t(scaled[0])<<24) | (uint32_t(scaled[1])<<16) |(uint32_t(scaled[2])<<8) |(uint32_t(scaled[3])<<0);
            }

            inline uint32_t ToARGB32( const Color& color )
            {
                Vector4i scaled = (color * 255).cast<int>();
                return (uint32_t(scaled[3])<<24) | (uint32_t(scaled[0])<<16) |(uint32_t(scaled[1])<<8) |(uint32_t(scaled[2])<<0);

            }

        }
    }
}


#endif //RADIUMENGINE_COLOR_PRESET_HPP_
