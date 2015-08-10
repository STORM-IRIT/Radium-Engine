#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>

namespace Ra
{
    namespace Engine
    {

        void RenderParameters::bind( ShaderProgram* shader ) const
        {
            m_intParamsVector.bind( shader );
            m_uintParamsVector.bind( shader );
            m_scalarParamsVector.bind( shader );

            m_vec2ParamsVector.bind( shader );
            m_vec3ParamsVector.bind( shader );
            m_vec4ParamsVector.bind( shader );

            m_mat2ParamsVector.bind( shader );
            m_mat3ParamsVector.bind( shader );
            m_mat4ParamsVector.bind( shader );

            m_texParamsVector.bind( shader );
        }

        void RenderParameters::addParameter( const char* name, int value )
        {
            m_intParamsVector.push_back( IntParameter( name, value ) );
        }

        void RenderParameters::addParameter( const char* name, uint value )
        {
            m_uintParamsVector.push_back( UIntParameter( name, value ) );
        }

        void RenderParameters::addParameter( const char* name, Scalar value )
        {
            m_scalarParamsVector.push_back( ScalarParameter( name, value ) );
        }

        void RenderParameters::addParameter( const char* name, const Core::Vector2& value )
        {
            m_vec2ParamsVector.push_back( Vec2Parameter( name, value ) );
        }

        void RenderParameters::addParameter( const char* name, const Core::Vector3& value )
        {
            m_vec3ParamsVector.push_back( Vec3Parameter( name, value ) );
        }

        void RenderParameters::addParameter( const char* name, const Core::Vector4& value )
        {
            m_vec4ParamsVector.push_back( Vec4Parameter( name, value ) );
        }

        void RenderParameters::addParameter( const char* name, const Core::Matrix2& value )
        {
            m_mat2ParamsVector.push_back( Mat2Parameter( name, value ) );
        }

        void RenderParameters::addParameter( const char* name, const Core::Matrix3& value )
        {
            m_mat3ParamsVector.push_back( Mat3Parameter( name, value ) );
        }

        void RenderParameters::addParameter( const char* name, const Core::Matrix4& value )
        {
            m_mat4ParamsVector.push_back( Mat4Parameter( name, value ) );
        }

        void RenderParameters::addParameter( const char* name, Texture* tex, int texUnit )
        {
            m_texParamsVector.push_back( TextureParameter( name, tex, texUnit ) );
        }

    } // namespace Engine
} // namespace Ra
