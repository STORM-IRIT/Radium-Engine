#ifndef RADIUMENGINE_GLMADAPTERS_H
#define RADIUMENGINE_GLMADAPTERS_H

#include <glm/glm.hpp>

#include <Core/Math/LinearAlgebra.hpp>

namespace Ra
{
    namespace Core
    {
        // Transform an Eigen vector/matrix into a glm vector/matrix.
        // Used to deal with globjects's setUniform() method which uses glm structures instead of Eigen's structures.

        inline glm::vec2 toGlm( const Vector2f& v )
        {
            return glm::vec2( v( 0 ), v( 1 ) );
        }

        inline glm::vec3 toGlm( const Vector3f& v )
        {
            return glm::vec3( v( 0 ), v( 1 ), v( 2 ) );
        }

        inline glm::vec4 toGlm( const Vector4f& v )
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

        inline glm::mat2 toGlm( const Matrix2f& m )
        {
            return glm::mat2 ( m.coeff( 0, 0 ), m.coeff( 1, 0 ),
                               m.coeff( 0, 1 ), m.coeff( 1, 1 ) );
        }

        inline glm::mat3 toGlm( const Matrix3f& m )
        {
            return glm::mat3( m.coeff( 0, 0 ), m.coeff( 1, 0 ), m.coeff( 2, 0 ),
                              m.coeff( 0, 1 ), m.coeff( 1, 1 ), m.coeff( 2, 1 ),
                              m.coeff( 0, 2 ), m.coeff( 1, 2 ), m.coeff( 2, 2 ) );
        }

        inline glm::mat4 toGlm( const Matrix4f& m )
        {
            return glm::mat4( m.coeff( 0, 0 ), m.coeff( 1, 0 ), m.coeff( 2, 0 ), m.coeff( 3, 0 ),
                              m.coeff( 0, 1 ), m.coeff( 1, 1 ), m.coeff( 2, 1 ), m.coeff( 3, 1 ),
                              m.coeff( 0, 2 ), m.coeff( 1, 2 ), m.coeff( 2, 2 ), m.coeff( 3, 2 ),
                              m.coeff( 0, 3 ), m.coeff( 1, 3 ), m.coeff( 2, 3 ), m.coeff( 3, 3 ) );
        }

        inline glm::mat2x3 toGlm( const Eigen::Matrix<float, 2, 3>& m )
        {
            return glm::mat2x3( m.coeff( 0, 0 ), m.coeff( 1, 0 ), m.coeff( 2, 0 ),
                                m.coeff( 0, 1 ), m.coeff( 1, 1 ), m.coeff( 2, 1 ) );
        }

        inline glm::mat3x2 toGlm( const Eigen::Matrix<float, 3, 2>& m )
        {
            return glm::mat3x2( m.coeff( 0, 0 ), m.coeff( 1, 0 ),
                                m.coeff( 0, 1 ), m.coeff( 1, 1 ),
                                m.coeff( 0, 2 ), m.coeff( 1, 2 ) );
        }

        inline glm::mat2x4 toGlm( const Eigen::Matrix<float, 2, 4>& m )
        {
            return glm::mat2x4( m.coeff( 0, 0 ), m.coeff( 1, 0 ), m.coeff( 2, 0 ), m.coeff( 3, 0 ),
                                m.coeff( 0, 1 ), m.coeff( 1, 1 ), m.coeff( 2, 1 ), m.coeff( 3, 1 ) );
        }

        inline glm::mat4x2 toGlm( const Eigen::Matrix<float, 4, 2>& m )
        {
            return glm::mat4x2( m.coeff( 0, 0 ), m.coeff( 1, 0 ),
                                m.coeff( 0, 1 ), m.coeff( 1, 1 ),
                                m.coeff( 0, 2 ), m.coeff( 1, 2 ),
                                m.coeff( 0, 3 ), m.coeff( 1, 3 ) );
        }

        inline glm::mat3x4 toGlm( const Eigen::Matrix<float, 3, 4>& m )
        {
            return glm::mat3x4( m.coeff( 0, 0 ), m.coeff( 1, 0 ), m.coeff( 2, 0 ), m.coeff( 3, 0 ),
                                m.coeff( 0, 1 ), m.coeff( 1, 1 ), m.coeff( 2, 1 ), m.coeff( 3, 1 ),
                                m.coeff( 0, 2 ), m.coeff( 1, 2 ), m.coeff( 2, 2 ), m.coeff( 3, 2 ) );
        }

        inline glm::mat4x3 toGlm( const Eigen::Matrix<float, 4, 3>& m )
        {
            return glm::mat4x3( m.coeff( 0, 0 ), m.coeff( 1, 0 ), m.coeff( 2, 0 ),
                                m.coeff( 0, 1 ), m.coeff( 1, 1 ), m.coeff( 2, 1 ),
                                m.coeff( 0, 2 ), m.coeff( 1, 2 ), m.coeff( 2, 2 ),
                                m.coeff( 0, 3 ), m.coeff( 1, 3 ), m.coeff( 2, 3 ) );
        }

        // Transform a glm vector/matrix into an Eigen vector/matrix.
        // It may be useful because globjects's getUniform() method returns a glm structure and Radium
        // only deals with Eigen's structures.

        inline void fromGlm( const glm::vec2& v, Vector2& out )
        {
            out << v[0], v[1];
        }

        inline Vector2 fromGlm( const glm::vec2& v )
        {
            return Vector2( v[0], v[1] );
        }

        inline void fromGlm( const glm::vec3& v, Vector3& out )
        {
            out << v[0], v[1], v[2];
        }

        inline Vector3 fromGlm( const glm::vec3& v )
        {
            return Vector3( v[0], v[1], v[2] );
        }

        inline void fromGlm( const glm::vec4& v, Vector4& out )
        {
            out << v[0], v[1], v[2], v[3];
        }

        inline Vector4 fromGlm( const glm::vec4& v )
        {
            return Vector4( v[0], v[1], v[2], v[3] );
        }

        inline void fromGlm( const glm::ivec2& v, Vector2i& out )
        {
            out << v[0], v[1];
        }

        inline Vector2i fromGlm( const glm::ivec2& v )
        {
            return Vector2i( v[0], v[1] );
        }

        inline void fromGlm( const glm::ivec3& v, Vector3i& out )
        {
            out << v[0], v[1], v[2];
        }

        inline Vector3i fromGlm( const glm::ivec3& v )
        {
            return Vector3i( v[0], v[1], v[2] );
        }

        inline void fromGlm( const glm::ivec4& v, Vector4i& out )
        {
            out << v[0], v[1], v[2], v[3];
        }

        inline Vector4i fromGlm( const glm::ivec4& v )
        {
            return Vector4i( v[0], v[1], v[2], v[3] );
        }

        inline void fromGlm( const glm::uvec2& v, Vector2ui& out )
        {
            out << v[0], v[1];
        }

        inline Vector2ui fromGlm( const glm::uvec2& v )
        {
            return Vector2ui( v[0], v[1] );
        }

        inline void fromGlm( const glm::uvec3& v, Vector3ui& out )
        {
            out << v[0], v[1], v[2];
        }

        inline Vector3ui fromGlm( const glm::uvec3& v )
        {
            return Vector3ui( v[0], v[1], v[2] );
        }

        inline void fromGlm( const glm::uvec4& v, Vector4ui& out )
        {
            out << v[0], v[1], v[2], v[3];
        }

        inline Vector4ui fromGlm( const glm::uvec4& v )
        {
            return Vector4ui( v[0], v[1], v[2], v[3] );
        }

        inline void fromGlm( const glm::mat2& m, Matrix2& out )
        {
            out << m[0][0], m[1][0],
                   m[0][1], m[1][1];
        }

        inline Matrix2 fromGlm( const glm::mat2& m )
        {
            Matrix2 returnMatrix;
            fromGlm( m, returnMatrix );
            return returnMatrix;
        }

        inline void fromGlm( const glm::mat3& m, Matrix3& out )
        {
            out << m[0][0], m[1][0], m[2][0],
                   m[0][1], m[1][1], m[2][1],
                   m[0][2], m[1][2], m[2][2];
        }

        inline Matrix3 fromGlm( const glm::mat3& m )
        {
            Matrix3 returnMatrix;
            fromGlm( m, returnMatrix );
            return returnMatrix;
        }

        inline void fromGlm( const glm::mat4& m, Matrix4& out )
        {
            out << m[0][0], m[1][0], m[2][0], m[3][0],
                   m[0][1], m[1][1], m[2][1], m[3][1],
                   m[0][2], m[1][2], m[2][2], m[3][2],
                   m[0][3], m[1][3], m[2][3], m[3][3];
        }

        inline Matrix4 fromGlm( const glm::mat4& m )
        {
            Matrix4 returnMatrix;
            fromGlm( m, returnMatrix );
            return returnMatrix;
        }

        inline void fromGlm( const glm::mat2x3& m, Eigen::Matrix<Scalar, 2, 3>& out )
        {
            out << m[0][0], m[1][0], m[2][0],
                   m[0][1], m[1][1], m[2][1];
        }

        inline Eigen::Matrix<Scalar, 2, 3> fromGlm( const glm::mat2x3& m )
        {
            Eigen::Matrix<Scalar, 2, 3> returnMatrix;
            fromGlm( m, returnMatrix );
            return returnMatrix;
        }

        inline void fromGlm( const glm::mat3x2& m, Eigen::Matrix<Scalar, 3, 2>& out )
        {
            out << m[0][0], m[1][0],
                   m[0][1], m[1][1],
                   m[0][2], m[1][2];
        }

        inline Eigen::Matrix<Scalar, 3, 2> fromGlm( const glm::mat3x2& m )
        {
            Eigen::Matrix<Scalar, 3, 2> returnMatrix;
            fromGlm( m, returnMatrix );
            return returnMatrix;
        }

        inline void fromGlm( const glm::mat2x4& m, Eigen::Matrix<Scalar, 2, 4>& out )
        {
            out << m[0][0], m[1][0], m[2][0], m[3][0],
                   m[0][1], m[1][1], m[2][1], m[3][1];
        }

        inline Eigen::Matrix<Scalar, 2, 4> fromGlm( const glm::mat2x4& m )
        {
            Eigen::Matrix<Scalar, 2, 4> returnMatrix;
            fromGlm( m, returnMatrix );
            return returnMatrix;
        }

        inline void fromGlm( const glm::mat4x2& m, Eigen::Matrix<Scalar, 4, 2>& out )
        {
            out << m[0][0], m[1][0],
                   m[0][1], m[1][1],
                   m[0][2], m[1][2],
                   m[0][3], m[1][3];
        }

        inline Eigen::Matrix<Scalar, 4, 2> fromGlm( const glm::mat4x2& m )
        {
            Eigen::Matrix<Scalar, 4, 2> returnMatrix;
            fromGlm( m, returnMatrix );
            return returnMatrix;
        }

        inline void fromGlm( const glm::mat3x4& m, Eigen::Matrix<Scalar, 3, 4>& out )
        {
            out << m[0][0], m[1][0], m[2][0], m[3][0],
                   m[0][1], m[1][1], m[2][1], m[3][1],
                   m[0][2], m[1][2], m[2][2], m[3][2];
        }

        inline Eigen::Matrix<Scalar, 3, 4> fromGlm( const glm::mat3x4& m )
        {
            Eigen::Matrix<Scalar, 3, 4> returnMatrix;
            fromGlm( m, returnMatrix );
            return returnMatrix;
        }

        inline void fromGlm( const glm::mat4x3& m, Eigen::Matrix<Scalar, 4, 3>& out )
        {
            out << m[0][0], m[1][0], m[2][0],
                   m[0][1], m[1][1], m[2][1],
                   m[0][2], m[1][2], m[2][2],
                   m[0][3], m[1][3], m[2][3];
        }

        inline Eigen::Matrix<Scalar, 4, 3> fromGlm( const glm::mat4x3& m )
        {
            Eigen::Matrix<Scalar, 4, 3> returnMatrix;
            fromGlm( m, returnMatrix );
            return returnMatrix;
        }
    }
}

#endif //RADIUMENGINE_GLMADAPTERS_H
