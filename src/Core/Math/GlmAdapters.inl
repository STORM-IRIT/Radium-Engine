#ifndef RADIUMENGINE_GLMADAPTERS_H
#define RADIUMENGINE_GLMADAPTERS_H

#include <glm/glm.hpp>

#include <Core/Math/LinearAlgebra.hpp>

namespace Ra
{
    namespace Core
    {
        // Transform an Eigen vector/matrix into a glm vector/matrix

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

        // Transform a glm vector/matrix into an Eigen vector/matrix

        inline Vector2 fromGlm( const glm::vec2& v )
        {
            return Vector2( v[0], v[1] );
        }

        inline Vector3 toGlm( const glm::vec3& v )
        {
            return Vector3( v[0], v[1], v[2] );
        }

        inline Vector4 toGlm( const glm::vec4& v )
        {
            return Vector4( v[0], v[1], v[2], v[3] );
        }

        inline Vector2i toGlm( const glm::ivec2& v )
        {
            return Vector2i( v[0], v[1] );
        }

        inline Vector3i toGlm( const glm::ivec3& v )
        {
            return Vector3i( v[0], v[1], v[2] );
        }

        inline Vector4i toGlm( const glm::ivec4& v )
        {
            return Vector4i( v[0], v[1], v[2], v[3] );
        }

        inline Vector2ui toGlm( const glm::uvec2& v )
        {
            return Vector2ui( v[0], v[1] );
        }

        inline Vector3ui toGlm( const glm::uvec3& v )
        {
            return Vector3ui( v[0], v[1], v[2] );
        }

        inline Vector4ui toGlm( const glm::uvec4& v )
        {
            return Vector4ui( v[0], v[1], v[2], v[3] );
        }

        inline Matrix2 toGlm( const glm::mat2& m )
        {
            return Matrix2 ( m[0][0], m[0][1],
                             m[1][0], m[1][1] );
        }

        inline Matrix3 toGlm( const glm::mat3& m )
        {
            return Matrix3 ( m[0][0], m[0][1], m[0][2],
                             m[1][0], m[1][1], m[1][2],
                             m[2][0], m[2][1], m[2][2] );
        }

        inline Matrix4 toGlm( const glm::mat4& m )
        {
            return Matrix4 ( m[0][0], m[0][1], m[0][2], m[0][3],
                             m[1][0], m[1][1], m[1][2], m[1][3],
                             m[2][0], m[2][1], m[2][2], m[2][3],
                             m[3][0], m[3][1], m[3][2], m[3][3] );
        }

        inline Eigen::Matrix<Scalar, 2, 3> toGlm( const glm::mat2x3& m )
        {
            return Eigen::Matrix<Scalar, 2, 3> ( m[0][0], m[0][1], m[0][2],
                                                 m[1][0], m[1][1], m[1][2] );
        }

        inline Eigen::Matrix<Scalar, 3, 2> toGlm( const glm::mat3x2& m )
        {
            return Eigen::Matrix<Scalar, 3, 2> ( m[0][0], m[0][1],
                                                 m[1][0], m[1][1],
                                                 m[2][0], m[2][1] );
        }

        inline Eigen::Matrix<Scalar, 2, 4> toGlm( const glm::mat2x4& m )
        {
            return Eigen::Matrix<Scalar, 2, 4> ( m[0][0], m[0][1], m[0][2], m[0][3],
                                                 m[1][0], m[1][1], m[1][2], m[1][3] );
        }

        inline Eigen::Matrix<Scalar, 4, 2> toGlm( const glm::mat4x2& m )
        {
            return Eigen::Matrix<Scalar, 4, 2> ( m[0][0], m[0][1],
                                                 m[1][0], m[1][1],
                                                 m[2][0], m[2][1],
                                                 m[3][0], m[3][1] );
        }

        inline Eigen::Matrix<Scalar, 3, 4> toGlm( const glm::mat3x4& m )
        {
            return Eigen::Matrix<Scalar, 3, 4> ( m[0][0], m[0][1], m[0][2], m[0][3],
                                                 m[1][0], m[1][1], m[1][2], m[1][3],
                                                 m[2][0], m[2][1], m[2][2], m[2][3] );
        }

        inline Eigen::Matrix<Scalar, 4, 3> toGlm( const glm::mat4x3& m )
        {
            return Eigen::Matrix<Scalar, 4, 3> ( m[0][0], m[0][1], m[0][2],
                                                 m[1][0], m[1][1], m[1][2],
                                                 m[2][0], m[2][1], m[2][2],
                                                 m[3][0], m[3][1], m[3][2] );
        }
    }
}

#endif //RADIUMENGINE_GLMADAPTERS_H
