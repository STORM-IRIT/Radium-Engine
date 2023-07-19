#pragma once
#include <Core/RaCore.hpp>

#include <array>
#include <cmath>

#include <Core/Types.hpp>

namespace Ra {
namespace Core {
namespace Material {

/**
 * \brief Implementation of the gltf_KHRTextureTransform extension.
 */
struct RA_CORE_API GLTFTextureTransform {
    std::array<Scalar, 2> offset { 0_ra, 0_ra };
    std::array<Scalar, 2> scale { 1_ra, 1_ra };
    Scalar rotation { 0_ra };
    int texCoord { -1 };

    /// Warning : this transformation take into account uv origin changes from gltf and Radium ...
    Ra::Core::Matrix3 getTransformationAsMatrix() const {
        Ra::Core::Matrix3 Mat_translation;
        Mat_translation << Scalar( 1 ), Scalar( 0 ), offset[0], Scalar( 0 ), Scalar( 1 ),
            -offset[1], Scalar( 0 ), Scalar( 0 ), Scalar( 1 );
        Ra::Core::Matrix3 Mat_rotation;
        Mat_rotation << std::cos( rotation ), -std::sin( rotation ), std::sin( rotation ),
            std::sin( rotation ), std::cos( rotation ), 1_ra - std::cos( rotation ), Scalar( 0 ),
            Scalar( 0 ), Scalar( 1 );
        Ra::Core::Matrix3 Mat_scale;
        Mat_scale << scale[0], Scalar( 0 ), Scalar( 0 ), Scalar( 0 ), scale[1], 1 - scale[1],
            Scalar( 0 ), Scalar( 0 ), Scalar( 1 );
        Ra::Core::Matrix3 res = Mat_translation * Mat_rotation * Mat_scale;
        return res;
    }
};

/**
 * \brief Sampler Data as defined by GlTF specification
 * Enums correspond to OpenGL specification
 */
struct RA_CORE_API GLTFSampler {
    enum class MagFilter : uint16_t { Nearest = 9728, Linear = 9729 };

    enum class MinFilter : uint16_t {
        Nearest              = 9728,
        Linear               = 9729,
        NearestMipMapNearest = 9984,
        LinearMipMapNearest  = 9985,
        NearestMipMapLinear  = 9986,
        LinearMipMapLinear   = 9987
    };

    enum class WrappingMode : uint16_t {
        ClampToEdge    = 33071,
        MirroredRepeat = 33648,
        Repeat         = 10497
    };

    MagFilter magFilter { MagFilter::Nearest };
    MinFilter minFilter { MinFilter::Nearest };

    WrappingMode wrapS { WrappingMode::Repeat };
    WrappingMode wrapT { WrappingMode::Repeat };
};

} // namespace Material
} // namespace Core
} // namespace Ra
