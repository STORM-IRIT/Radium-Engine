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
            template <typename C = Color> inline C Alpha() { return C(0.0, 0.0, 0.0, 0.0); }
            template <typename C = Color> inline C Black() { return C(0,0,0,1); }

            template <typename C = Color> inline C Red()   { return C(1,0,0,1); }
            template <typename C = Color> inline C Green() { return C(0,1,0,1); }
            template <typename C = Color> inline C Blue()  { return C(0,0,1,1); }

            template <typename C = Color> inline C Yellow()  { return C(1,1,0,1); }
            template <typename C = Color> inline C Magenta() { return C(1,0,1,1); }
            template <typename C = Color> inline C Cyan()    { return C(0,1,1,1); }

            template <typename C = Color> inline C White()   { return C(1,1,1,1); }

            template <typename C = Color> inline C Grey( Scalar f = 0.5f) { return Color(f,f,f,1);}

            template <typename C = Color> inline C Skin() { return Color(1.0,0.87,0.74,1.0);}
            // Convert to/from various int formats
            template <typename C = Color>
            inline C FromChars(uchar r, uchar g, uchar b, uchar a = 0xff)
            {
                return C(Scalar(r)/255.0f, Scalar(g)/255.0f, Scalar(b)/255.0f, Scalar(a)/255.0f );
            }

            template <typename C = Color>
            inline C FromRGBA32(uint32_t rgba)
            {
                uchar r = uchar((rgba >> 24) & 0xff);
                uchar g = uchar((rgba >> 16) & 0xff);
                uchar b = uchar((rgba >>  8) & 0xff);
                uchar a = uchar((rgba >>  0) & 0xff);
                return FromChars(r,g,b,a);
            }

            template <typename C = Color>
            inline C FromARGB32(uint32_t argb)
            {
                uchar a = uchar((argb >> 24) & 0xff);
                uchar r = uchar((argb >> 16) & 0xff);
                uchar g = uchar((argb >>  8) & 0xff);
                uchar b = uchar((argb >>  0) & 0xff);
                return FromChars(r,g,b,a);
            }

            template <typename C = Color>
            inline C fromHSV( const Scalar hue,
                              const Scalar saturation = 1.0,
                              const Scalar value      = 1.0,
                              const Scalar alpha      = 1.0 ) {
                C c;

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

            template <typename C = Color>
            inline uint32_t ToRGBA32(const C& color)
            {
                C c(color * 255);
                Vector4i scaled(c.x(), c.y(), c.z(), c.w());
                return (uint32_t(scaled[0])<<24) | (uint32_t(scaled[1])<<16) |(uint32_t(scaled[2])<<8) |(uint32_t(scaled[3])<<0);
            }

            template <typename C = Color>
            inline uint32_t ToARGB32( const C& color )
            {
                C c(color * 255);
                Vector4i scaled(c.x(), c.y(), c.z(), c.w());
                return (uint32_t(scaled[3])<<24) | (uint32_t(scaled[0])<<16) |(uint32_t(scaled[1])<<8) |(uint32_t(scaled[2])<<0);
            }

        }
    }
}


#endif //RADIUMENGINE_COLOR_PRESET_HPP_
