#ifndef RADIUMENGINE_GLMADAPTERS_H
#define RADIUMENGINE_GLMADAPTERS_H

#include <glm/glm.hpp>

#include <Core/Math/LinearAlgebra.hpp>

namespace Ra
{
    namespace Core
    {
        inline glm::vec2 toGlm( const Vector2& v )
        {
            return glm::vec2( v( 0 ), v( 1 ) );
        }

        inline glm::vec3 toGlm( const Vector3& v )
        {
            return glm::vec3( v( 0 ), v( 1 ), v( 2 ) );
        }

        inline glm::vec4 toGlm( const Vector4& v )
        {
            return glm::vec4( v( 0 ), v( 1 ), v( 2 ), v( 3 ) );
        }

        inline glm::ivec2 toGlm( const Vector2i& v )
        {
            return glm::ivec2( v( 0 ), v( 1 ) );
        }

        inline glm::ivec3 toGlm( const Vector3i& v )
        {
            return glm::ivec3( v( 0 ), v( 1 ), v( 2 ) );
        }

        inline glm::ivec4 toGlm( const Vector4i& v )
        {
            return glm::ivec4( v( 0 ), v( 1 ), v( 2 ), v( 3 ) );
        }

        inline glm::uvec2 toGlm( const Vector2ui& v )
        {
            return glm::uvec2( v( 0 ), v( 1 ) );
        }

        inline glm::uvec3 toGlm( const Vector3ui& v )
        {
            return glm::uvec3( v( 0 ), v( 1 ), v( 2 ) );
        }

        inline glm::uvec4 toGlm( const Vector4ui& v )
        {
            return glm::uvec4( v( 0 ), v( 1 ), v( 2 ), v( 3 ) );
        }

        inline glm::mat2 toGlm( const Matrix2& m )
        {
            return glm::mat2 ( m.coeff( 0, 0 ), m.coeff( 0, 1 ),
                               m.coeff( 1, 0 ), m.coeff( 1, 1 ) );
        }

        inline glm::mat3 toGlm( const Matrix3& m )
        {
            return glm::mat3( m.coeff( 0, 0 ), m.coeff( 0, 1 ), m.coeff( 0, 2 ),
                              m.coeff( 1, 0 ), m.coeff( 1, 1 ), m.coeff( 1, 2 ),
                              m.coeff( 2, 0 ), m.coeff( 2, 1 ), m.coeff( 2, 2 ) );
        }

        inline glm::mat4 toGlm( const Matrix4& m )
        {
            return glm::mat4( m.coeff( 0, 0 ), m.coeff( 0, 1 ), m.coeff( 0, 2 ), m.coeff( 0, 3 ),
                              m.coeff( 1, 0 ), m.coeff( 1, 1 ), m.coeff( 1, 2 ), m.coeff( 1, 3 ),
                              m.coeff( 2, 0 ), m.coeff( 2, 1 ), m.coeff( 2, 2 ), m.coeff( 2, 3 ),
                              m.coeff( 3, 0 ), m.coeff( 3, 1 ), m.coeff( 3, 2 ), m.coeff( 3, 3 ) );
        }

        inline glm::mat2x3 toGlm( const Eigen::Matrix<Scalar, 2, 3>& m )
        {
            return glm::mat2x3( m.coeff( 0, 0 ), m.coeff( 0, 1 ), m.coeff( 0, 2 ),
                                m.coeff( 1, 0 ), m.coeff( 1, 1 ), m.coeff( 1, 2 ) );
        }

        inline glm::mat3x2 toGlm( const Eigen::Matrix<Scalar, 3, 2>& m )
        {
            return glm::mat3x2( m.coeff( 0, 0 ), m.coeff( 0, 1 ),
                                m.coeff( 1, 0 ), m.coeff( 1, 1 ),
                                m.coeff( 2, 0 ), m.coeff( 2, 1 ) );
        }

        inline glm::mat2x4 toGlm( const Eigen::Matrix<Scalar, 2, 4>& m )
        {
            return glm::mat2x4( m.coeff( 0, 0 ), m.coeff( 0, 1 ), m.coeff( 0, 2 ), m.coeff( 0, 3 ),
                                m.coeff( 1, 0 ), m.coeff( 1, 1 ), m.coeff( 1, 2 ), m.coeff( 1, 3 ) );
        }

        inline glm::mat4x2 toGlm( const Eigen::Matrix<Scalar, 4, 2>& m )
        {
            return glm::mat4x2( m.coeff( 0, 0 ), m.coeff( 0, 1 ),
                                m.coeff( 1, 0 ), m.coeff( 1, 1 ),
                                m.coeff( 2, 0 ), m.coeff( 2, 1 ),
                                m.coeff( 3, 0 ), m.coeff( 3, 1 ) );
        }

        inline glm::mat3x4 toGlm( const Eigen::Matrix<Scalar, 3, 4>& m )
        {
            return glm::mat3x4( m.coeff( 0, 0 ), m.coeff( 0, 1 ), m.coeff( 0, 2 ), m.coeff( 0, 3 ),
                                m.coeff( 1, 0 ), m.coeff( 1, 1 ), m.coeff( 1, 2 ), m.coeff( 1, 3 ),
                                m.coeff( 2, 0 ), m.coeff( 2, 1 ), m.coeff( 2, 2 ), m.coeff( 2, 3 ) );
        }

        inline glm::mat4x3 toGlm( const Eigen::Matrix<Scalar, 4, 3>& m )
        {
            return glm::mat4x3( m.coeff( 0, 0 ), m.coeff( 0, 1 ), m.coeff( 0, 2 ),
                                m.coeff( 1, 0 ), m.coeff( 1, 1 ), m.coeff( 1, 2 ),
                                m.coeff( 2, 0 ), m.coeff( 2, 1 ), m.coeff( 2, 2 ),
                                m.coeff( 3, 0 ), m.coeff( 3, 1 ), m.coeff( 3, 2 ) );
        }
    }
}

#endif //RADIUMENGINE_GLMADAPTERS_H
