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
