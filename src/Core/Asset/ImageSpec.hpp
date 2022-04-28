// Copyright 2008-present Contributors to the OpenImageIO project.
// SPDX-License-Identifier: BSD-3-Clause
// https://github.com/OpenImageIO/oiio

#include <map>

#pragma once
#include <vector>

#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace Asset {
// namespace ImageSpec {

/////////////////////////////////////////////////////////////////////////////////
// header from OpenImageIO : typedesc.h

/////////////////////////////////////////////////////////////////////////////
/// A TypeDesc describes simple data types.
///
/// It frequently comes up (in my experience, with renderers and image
/// handling programs) that you want a way to describe data that is passed
/// through APIs through blind pointers.  These are some simple classes
/// that provide a simple type descriptor system.  This is not meant to
/// be comprehensive -- for example, there is no provision for structs,
/// unions, pointers, const, or 'nested' type definitions.  Just simple
/// integer and floating point, *common* aggregates such as 3-points,
/// and reasonably-lengthed arrays thereof.
///
/////////////////////////////////////////////////////////////////////////////

struct RA_CORE_API TypeDesc {
    /// BASETYPE is a simple enum describing the base data types that
    /// correspond (mostly) to the C/C++ built-in types.
    enum BASETYPE {
        UNKNOWN, ///< unknown type
        NONE,    ///< void/no type
        UINT8,   ///< 8-bit unsigned int values ranging from 0..255,
                 ///<   (C/C++ `unsigned char`).
        UCHAR = UINT8,
        INT8, ///< 8-bit int values ranging from -128..127,
              ///<   (C/C++ `char`).
        CHAR = INT8,
        UINT16, ///< 16-bit int values ranging from 0..65535,
                ///<   (C/C++ `unsigned short`).
        USHORT = UINT16,
        INT16, ///< 16-bit int values ranging from -32768..32767,
               ///<   (C/C++ `short`).
        SHORT = INT16,
        UINT32, ///< 32-bit unsigned int values (C/C++ `unsigned int`).
        UINT = UINT32,
        INT32, ///< signed 32-bit int values (C/C++ `int`).
        INT = INT32,
        UINT64, ///< 64-bit unsigned int values (C/C++
                ///<   `unsigned long long` on most architectures).
        ULONGLONG = UINT64,
        INT64, ///< signed 64-bit int values (C/C++ `long long`
               ///<   on most architectures).
        LONGLONG = INT64,
        HALF,   ///< 16-bit IEEE floating point values (OpenEXR `half`).
        FLOAT,  ///< 32-bit IEEE floating point values, (C/C++ `float`).
        DOUBLE, ///< 64-bit IEEE floating point values, (C/C++ `double`).
        STRING, ///< Character string.
        PTR,    ///< A pointer value.
        LASTBASE
    };

    /// AGGREGATE describes whether our TypeDesc is a simple scalar of one
    /// of the BASETYPE's, or one of several simple aggregates.
    ///
    /// Note that aggregates and arrays are different. A `TypeDesc(FLOAT,3)`
    /// is an array of three floats, a `TypeDesc(FLOAT,VEC3)` is a single
    /// 3-component vector comprised of floats, and `TypeDesc(FLOAT,3,VEC3)`
    /// is an array of 3 vectors, each of which is comprised of 3 floats.
    enum AGGREGATE {
        SCALAR = 1,   ///< A single scalar value (such as a raw `int` or
                      ///<   `float` in C).  This is the default.
        VEC2     = 2, ///< 2 values representing a 2D vector.
        VEC3     = 3, ///< 3 values representing a 3D vector.
        VEC4     = 4, ///< 4 values representing a 4D vector.
        MATRIX33 = 9, ///< 9 values representing a 3x3 matrix.
        MATRIX44 = 16 ///< 16 values representing a 4x4 matrix.
    };

    /// VECSEMANTICS gives hints about what the data represent (for example,
    /// if a spatial vector quantity should transform as a point, direction
    /// vector, or surface normal).
    enum VECSEMANTICS {
        NOXFORM     = 0, ///< No semantic hints.
        NOSEMANTICS = 0, ///< No semantic hints.
        COLOR,           ///< Color
        POINT,           ///< Point: a spatial location
        VECTOR,          ///< Vector: a spatial direction
        NORMAL,          ///< Normal: a surface normal
        TIMECODE,        ///< indicates an `int[2]` representing the standard
                         ///<   4-byte encoding of an SMPTE timecode.
        KEYCODE,         ///< indicates an `int[7]` representing the standard
                         ///<   28-byte encoding of an SMPTE keycode.
        RATIONAL,        ///< A VEC2 representing a rational number `val[0] / val[1]`
        BOX,             ///< A VEC2[2] or VEC3[2] that represents a 2D or 3D bounds (min/max)
    };

    unsigned char basetype;     ///< C data type at the heart of our type
    unsigned char aggregate;    ///< What kind of AGGREGATE is it?
    unsigned char vecsemantics; ///< Hint: What does the aggregate represent?
    unsigned char reserved;     ///< Reserved for future expansion
    int arraylen;               ///< Array length, 0 = not array, -1 = unsized

    bool operator<(const TypeDesc& other) const {
        return basetype * aggregate < other.basetype * other.aggregate;
    }

    /// Construct from a BASETYPE and optional aggregateness, semantics,
    /// and arrayness.
    constexpr TypeDesc( BASETYPE btype         = UNKNOWN,
                        AGGREGATE agg          = SCALAR,
                        VECSEMANTICS semantics = NOSEMANTICS,
                        int arraylen           = 0 ) noexcept :
        basetype( static_cast<unsigned char>( btype ) ),
        aggregate( static_cast<unsigned char>( agg ) ),
        vecsemantics( static_cast<unsigned char>( semantics ) ),
        reserved( 0 ),
        arraylen( arraylen ) {}

    //    /// Copy constructor.
    //    constexpr TypeDesc (const TypeDesc &t) noexcept
    //        : basetype(t.basetype), aggregate(t.aggregate),
    //          vecsemantics(t.vecsemantics), reserved(0), arraylen(t.arraylen)
    //          { }
};

// Static values for commonly used types. Because these are constexpr,
// they should incur no runtime construction cost and should optimize nicely
// in various ways.
constexpr TypeDesc TypeUnknown( TypeDesc::UNKNOWN );
constexpr TypeDesc TypeFloat( TypeDesc::FLOAT );
constexpr TypeDesc TypeColor( TypeDesc::FLOAT, TypeDesc::VEC3, TypeDesc::COLOR );
constexpr TypeDesc TypePoint( TypeDesc::FLOAT, TypeDesc::VEC3, TypeDesc::POINT );
constexpr TypeDesc TypeVector( TypeDesc::FLOAT, TypeDesc::VEC3, TypeDesc::VECTOR );
constexpr TypeDesc TypeNormal( TypeDesc::FLOAT, TypeDesc::VEC3, TypeDesc::NORMAL );
constexpr TypeDesc TypeMatrix33( TypeDesc::FLOAT, TypeDesc::MATRIX33 );
constexpr TypeDesc TypeMatrix44( TypeDesc::FLOAT, TypeDesc::MATRIX44 );
constexpr TypeDesc TypeMatrix = TypeMatrix44;
constexpr TypeDesc TypeFloat2( TypeDesc::FLOAT, TypeDesc::VEC2 );
constexpr TypeDesc TypeVector2( TypeDesc::FLOAT, TypeDesc::VEC2, TypeDesc::VECTOR );
constexpr TypeDesc TypeFloat4( TypeDesc::FLOAT, TypeDesc::VEC4 );
constexpr TypeDesc TypeVector4 = TypeFloat4;
constexpr TypeDesc TypeString( TypeDesc::STRING );
constexpr TypeDesc TypeInt( TypeDesc::INT );
constexpr TypeDesc TypeUInt( TypeDesc::UINT );
constexpr TypeDesc TypeInt32( TypeDesc::INT );
constexpr TypeDesc TypeUInt32( TypeDesc::UINT );
constexpr TypeDesc TypeInt16( TypeDesc::INT16 );
constexpr TypeDesc TypeUInt16( TypeDesc::UINT16 );
constexpr TypeDesc TypeInt8( TypeDesc::INT8 );
constexpr TypeDesc TypeUInt8( TypeDesc::UINT8 );
constexpr TypeDesc TypeInt64( TypeDesc::INT64 );
constexpr TypeDesc TypeUInt64( TypeDesc::UINT64 );
constexpr TypeDesc TypeVector2i( TypeDesc::INT, TypeDesc::VEC2 );
constexpr TypeDesc TypeBox2( TypeDesc::FLOAT, TypeDesc::VEC2, TypeDesc::BOX, 2 );
constexpr TypeDesc TypeBox3( TypeDesc::FLOAT, TypeDesc::VEC3, TypeDesc::BOX, 2 );
constexpr TypeDesc TypeBox2i( TypeDesc::INT, TypeDesc::VEC2, TypeDesc::BOX, 2 );
constexpr TypeDesc TypeBox3i( TypeDesc::INT, TypeDesc::VEC3, TypeDesc::BOX, 2 );
constexpr TypeDesc TypeHalf( TypeDesc::HALF );
constexpr TypeDesc TypeTimeCode( TypeDesc::UINT, TypeDesc::SCALAR, TypeDesc::TIMECODE, 2 );
constexpr TypeDesc TypeKeyCode( TypeDesc::INT, TypeDesc::SCALAR, TypeDesc::KEYCODE, 7 );
constexpr TypeDesc TypeRational( TypeDesc::INT, TypeDesc::VEC2, TypeDesc::RATIONAL );
constexpr TypeDesc TypePointer( TypeDesc::PTR );

static const std::map<TypeDesc, int> g_typeDesc2size = { { TypeFloat, 4 },
                                          { TypeColor, 4 * 3 },
                                          { TypePoint, 4 * 3 },
                                          { TypeVector, 4 * 3 },
                                          { TypeNormal, 4 * 3 },
                                          { TypeMatrix33, 4 * 3 * 3 },
                                          { TypeMatrix44, 4 * 4 * 4 },
                                          { TypeMatrix, 4 * 4 * 4 },
                                          { TypeFloat2, 4 * 2 },
                                          { TypeVector2, 4 * 2 },
                                          { TypeFloat4, 4 * 4 },
                                          { TypeVector4, 4 * 4 },
                                          { TypeInt, 4 },
                                          { TypeUInt, 4 },
                                          { TypeInt32, 4 },
                                          { TypeUInt32, 4 },
                                          { TypeInt16, 2 },
                                          { TypeUInt16, 2 },
                                          { TypeInt8, 1 },
                                          { TypeUInt8, 1 },
                                          { TypeInt64, 8 },
                                          { TypeUInt64, 8 },
                                          { TypeVector2i, 4 * 2 } };

/////////////////////////////////////////////////////////////////////////////////
// header from OpenImageIO : paramlist.h

/// ParamValue holds a parameter and a pointer to its value(s)
///
/// Nomenclature: if you have an array of 4 colors for each of 15 points...
///  - There are 15 VALUES
///  - Each value has an array of 4 ELEMENTS, each of which is a color
///  - A color has 3 COMPONENTS (R, G, B)
///
struct RA_CORE_API ParamValue {
    enum Interp {
        INTERP_CONSTANT = 0, ///< Constant for all pieces/faces
        INTERP_PERPIECE = 1, ///< Piecewise constant per piece/face
        INTERP_LINEAR   = 2, ///< Linearly interpolated across each piece/face
        INTERP_VERTEX   = 3  ///< Interpolated like vertices
    };

    std::string m_name; ///< data name
    TypeDesc m_type;    ///< data type, which may itself be an array
    union {
        char localval[16];
        const void* ptr;
    } m_data;                                 ///< Our data, either a pointer or small local value
    int m_nvalues          = 0;               ///< number of values of the given type
    unsigned char m_interp = INTERP_CONSTANT; ///< Interpolation type
    bool m_copy            = false;
    bool m_nonlocal        = false;
};
using ParamValueList = std::vector<ParamValue>;

/////////////////////////////////////////////////////////////////////////////////
// header from OpenImageIO : imagio.h

/// ImageSpec describes the data format of an image -- dimensions, layout,
/// number and meanings of image channels.
///
/// The `width, height, depth` are the size of the data of this image, i.e.,
/// the number of pixels in each dimension.  A ``depth`` greater than 1
/// indicates a 3D "volumetric" image. The `x, y, z` fields indicate the
/// *origin* of the pixel data of the image. These default to (0,0,0), but
/// setting them differently may indicate that this image is offset from the
/// usual origin.
/// Therefore the pixel data are defined over pixel coordinates
///    [`x` ... `x+width-1`] horizontally,
///    [`y` ... `y+height-1`] vertically,
///    and [`z` ... `z+depth-1`] in depth.
///
/// The analogous `full_width`, `full_height`, `full_depth` and `full_x`,
/// `full_y`, `full_z` fields define a "full" or "display" image window over
/// the region [`full_x` ... `full_x+full_width-1`] horizontally, [`full_y`
/// ... `full_y+full_height-1`] vertically, and [`full_z`...
/// `full_z+full_depth-1`] in depth.
///
/// Having the full display window different from the pixel data window can
/// be helpful in cases where you want to indicate that your image is a
/// *crop window* of a larger image (if the pixel data window is a subset of
/// the full display window), or that the pixels include *overscan* (if the
/// pixel data is a superset of the full display window), or may simply
/// indicate how different non-overlapping images piece together.
///
/// For tiled images, `tile_width`, `tile_height`, and `tile_depth` specify
/// that the image is stored in a file organized into rectangular *tiles*
/// of these dimensions. The default of 0 value for these fields indicates
/// that the image is stored in scanline order, rather than as tiles.
///

struct RA_CORE_API ImageSpec {
  public:
    ///@{
    /// @name ImageSpec data members
    ///
    /// The `ImageSpec` contains data fields for the values that are
    /// required to describe nearly any image, and an extensible list of
    /// arbitrary attributes that can hold metadata that may be user-defined
    /// or specific to individual file formats.
    ///
    /// Here are the hard-coded data fields:

    int x;           ///< origin (upper left corner) of pixel data
    int y;           ///< origin (upper left corner) of pixel data
    int z;           ///< origin (upper left corner) of pixel data
    int width;       ///< width of the pixel data window
    int height;      ///< height of the pixel data window
    int depth;       ///< depth of pixel data, >1 indicates a "volume"
    int full_x;      ///< origin of the full (display) window
    int full_y;      ///< origin of the full (display) window
    int full_z;      ///< origin of the full (display) window
    int full_width;  ///< width of the full (display) window
    int full_height; ///< height of the full (display) window
    int full_depth;  ///< depth of the full (display) window
    int tile_width;  ///< tile width (0 for a non-tiled image)
    int tile_height; ///< tile height (0 for a non-tiled image)
    int tile_depth;  ///< tile depth (0 for a non-tiled image,
                     ///<             1 for a non-volume image)
    int nchannels;   ///< number of image channels, e.g., 4 for RGBA

    TypeDesc format; ///< Data format of the channels.
                     ///< Describes the native format of the pixel data values
                     /// themselves, as a `TypeDesc`.  Typical values would be
                     /// `TypeDesc::UINT8` for 8-bit unsigned values, `TypeDesc::FLOAT`
                     /// for 32-bit floating-point values, etc.
    std::vector<TypeDesc> channelformats;
    ///< Optional per-channel data formats. If all channels of the image
    /// have the same data format, that will be described by `format`
    /// and `channelformats` will be empty (zero length). If there are
    /// different data formats for each channel, they will be described
    /// in the `channelformats` vector, and the `format` field will
    /// indicate a single default data format for applications that
    /// don't wish to support per-channel formats (usually this will be
    /// the format of the channel that has the most precision).

    std::vector<std::string> channelnames;
    ///< The names of each channel, in order. Typically this will be "R",
    ///< "G", "B", "A" (alpha), "Z" (depth), or other arbitrary names.
    int alpha_channel;
    ///< The index of the channel that represents *alpha* (pixel
    ///< coverage and/or transparency).  It defaults to -1 if no alpha
    ///< channel is present, or if it is not known which channel
    ///< represents alpha.
    int z_channel;
    ///< The index of the channel that represents *z* or *depth* (from
    ///< the camera).  It defaults to -1 if no depth channel is present,
    ///< or if it is not know which channel represents depth.
    bool deep; ///< True if the image contains deep data.
               ///< If `true`, this indicates that the image describes contains
               ///< "deep" data consisting of multiple samples per pixel.  If
               ///< `false`, it's an ordinary image with one data value (per
               ///< channel) per pixel.

    //    ParamValueList extra_attribs; // hard to serialize, no metadada for radium
    ///< A list of arbitrarily-named and arbitrarily-typed additional
    /// attributes of the image, for any metadata not described by the
    /// hard-coded fields described above.  This list may be manipulated
    /// with the `attribute()` and `find_attribute()` methods.

    /// Constructs an `ImageSpec` with the given x and y resolution, number
    /// of channels, and pixel data format.
    ///
    /// All other fields are set to the obvious defaults -- the image is an
    /// ordinary 2D image (not a volume), the image is not offset or a crop
    /// of a bigger image, the image is scanline-oriented (not tiled),
    /// channel names are "R", "G", "B"' and "A" (up to and including 4
    /// channels, beyond that they are named "channel *n*"), the fourth
    /// channel (if it exists) is assumed to be alpha.
    ImageSpec( int xres, int yres, int nchans, TypeDesc fmt = TypeUInt8 ) noexcept;

    /// Sets the `channelnames` to reasonable defaults for the number of
    /// channels.  Specifically, channel names are set to "R", "G", "B,"
    /// and "A" (up to and including 4 channels, beyond that they are named
    /// "channel*n*".
    void default_channel_names() noexcept;

    ///@}
};

//} // namespace ImageSpec
} // namespace Asset
} // namespace Core
} // namespace Ra
