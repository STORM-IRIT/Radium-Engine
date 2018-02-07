#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>
#include <Core/Log/Log.hpp>

namespace Ra
{
    namespace Engine
    {
        void RenderParameters::bind(const ShaderProgram* shader) const
        {
            m_intParamsVector.bind( shader );
            m_uintParamsVector.bind( shader );
            m_scalarParamsVector.bind( shader );

            m_intsParamsVector.bind( shader );
            m_uintsParamsVector.bind( shader );
            m_scalarsParamsVector.bind( shader );

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
            m_intParamsVector[name] = IntParameter( name, value );
        }

        void RenderParameters::addParameter( const char* name, uint value )
        {
            m_uintParamsVector[name] = UIntParameter( name, value );
        }

        void RenderParameters::addParameter( const char* name, Scalar value )
        {
            m_scalarParamsVector[name] = ScalarParameter( name, value );
        }
        
        ///!! array version

        void RenderParameters::addParameter( const char* name, std::vector<int> value )
        {
            m_intsParamsVector[name] = IntsParameter( name, value );
        }

        void RenderParameters::addParameter( const char* name, std::vector<uint> value )
        {
            m_uintsParamsVector[name] = UIntsParameter( name, value );
        }

        void RenderParameters::addParameter( const char* name, std::vector<Scalar> value )
        {
            m_scalarsParamsVector[name] = ScalarsParameter( name, value );
        }
        
        ///!!

        void RenderParameters::addParameter( const char* name, const Core::Vector2& value )
        {
            m_vec2ParamsVector[name] = Vec2Parameter( name, value );
        }

        void RenderParameters::addParameter( const char* name, const Core::Vector3& value )
        {
            m_vec3ParamsVector[name] = Vec3Parameter( name, value );
        }

        void RenderParameters::addParameter( const char* name, const Core::Vector4& value )
        {
            m_vec4ParamsVector[name] = Vec4Parameter( name, value );
        }

        void RenderParameters::addParameter( const char* name, const Core::Matrix2& value )
        {
            m_mat2ParamsVector[name] = Mat2Parameter( name, value );
        }

        void RenderParameters::addParameter( const char* name, const Core::Matrix3& value )
        {
            m_mat3ParamsVector[name] = Mat3Parameter( name, value );
        }

        void RenderParameters::addParameter( const char* name, const Core::Matrix4& value )
        {
            m_mat4ParamsVector[name] = Mat4Parameter( name, value );
        }

        void RenderParameters::addParameter( const char* name, Texture* tex, int texUnit )
        {
            m_texParamsVector[name] = TextureParameter( name, tex, texUnit );
        }

        void RenderParameters::updateParameter( const char* name, int    value )
        {
            m_intParamsVector[name] = IntParameter( name, value );
        }

        void RenderParameters::updateParameter( const char* name, uint   value )
        {
            m_uintParamsVector[name] = UIntParameter( name, value );
        }

        void RenderParameters::updateParameter( const char* name, Scalar value )
        {
            m_scalarParamsVector[name] = ScalarParameter( name, value );
        }

        void RenderParameters::updateParameter( const char* name, const Core::Vector2& value )
        {
            m_vec2ParamsVector[name] = Vec2Parameter( name, value );
        }

        void RenderParameters::updateParameter( const char* name, const Core::Vector3& value )
        {
            m_vec3ParamsVector[name] = Vec3Parameter( name, value );
        }

        void RenderParameters::updateParameter( const char* name, const Core::Vector4& value )
        {
            m_vec4ParamsVector[name] = Vec4Parameter( name, value );
        }

        void RenderParameters::updateParameter( const char* name, const Core::Matrix2& value )
        {
            m_mat2ParamsVector[name] = Mat2Parameter( name, value );
        }

        void RenderParameters::updateParameter( const char* name, const Core::Matrix3& value )
        {
            m_mat3ParamsVector[name] = Mat3Parameter( name, value );
        }

        void RenderParameters::updateParameter( const char* name, const Core::Matrix4& value )
        {
            m_mat4ParamsVector[name] = Mat4Parameter( name, value );
        }

        void RenderParameters::concatParameters( const RenderParameters &params )
        {
            for (const auto& param : params.m_intParamsVector)
            {
                m_intParamsVector.insert( param );
            }

            for (const auto& param : params.m_uintParamsVector)
            {
                m_uintParamsVector.insert( param );
            }

            for (const auto& param : params.m_scalarParamsVector)
            {
                m_scalarParamsVector.insert( param );
            }

            for (const auto& param : params.m_vec2ParamsVector)
            {
                m_vec2ParamsVector.insert( param );
            }

            for (const auto& param : params.m_vec3ParamsVector)
            {
                m_vec3ParamsVector.insert( param );
            }

            for (const auto& param : params.m_vec4ParamsVector)
            {
                m_vec4ParamsVector.insert( param );
            }

            for (const auto& param : params.m_mat2ParamsVector)
            {
                m_mat2ParamsVector.insert( param );
            }

            for (const auto& param : params.m_mat3ParamsVector)
            {
                m_mat3ParamsVector.insert( param );
            }

            for (const auto& param : params.m_mat4ParamsVector)
            {
                m_mat4ParamsVector.insert( param );
            }

            for (const auto& param : params.m_texParamsVector)
            {
                m_texParamsVector.insert( param );
            }
        }

    } // namespace Engine
} // namespace Ra
