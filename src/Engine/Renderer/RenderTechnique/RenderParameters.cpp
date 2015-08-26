#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>
#include <Core/Log/Log.hpp>

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

        void RenderParameters::concatParameters( const RenderParameters &params )
        {
            for (const auto& param : params.m_intParamsVector)
            {
                m_intParamsVector.push_back( param );
            }

            for (const auto& param : params.m_uintParamsVector)
            {
                m_uintParamsVector.push_back( param );
            }

            for (const auto& param : params.m_scalarParamsVector)
            {
                m_scalarParamsVector.push_back( param );
            }

            for (const auto& param : params.m_vec2ParamsVector)
            {
                m_vec2ParamsVector.push_back( param );
            }

            for (const auto& param : params.m_vec3ParamsVector)
            {
                m_vec3ParamsVector.push_back( param );
            }

            for (const auto& param : params.m_vec4ParamsVector)
            {
                m_vec4ParamsVector.push_back( param );
            }

            for (const auto& param : params.m_mat2ParamsVector)
            {
                m_mat2ParamsVector.push_back( param );
            }

            for (const auto& param : params.m_mat3ParamsVector)
            {
                m_mat3ParamsVector.push_back( param );
            }

            for (const auto& param : params.m_mat4ParamsVector)
            {
                m_mat4ParamsVector.push_back( param );
            }

            for (const auto& param : params.m_texParamsVector)
            {
                m_texParamsVector.push_back( param );
            }
        }

    } // namespace Engine
} // namespace Ra
