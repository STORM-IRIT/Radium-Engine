#ifndef RADIUMENGINE_RENDERPARAMETERS_HPP
#define RADIUMENGINE_RENDERPARAMETERS_HPP

#include <vector>

#include <Core/CoreMacros.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Containers/AlignedAllocator.hpp>

namespace Ra
{
    namespace Engine
    {
        class Texture;
        class ShaderProgram;
    }
}

namespace Ra
{
    namespace Engine
    {

        class RenderParameters
        {
        private:
            class Parameter
            {
            public:
                Parameter( const char* name ) : m_name( name ) {}
                virtual void bind( ShaderProgram* shader ) const = 0;

            protected:
                const char* m_name;
            };

            template <typename T>
            class TParameter : public Parameter
            {
            public:
                TParameter( const char* name, const T& value )
                    : Parameter( name ), m_value( value ) {}
                virtual void bind( ShaderProgram* shader ) const override;

            private:
                T m_value;
            };

            class TextureParameter : public Parameter
            {
            public:
                TextureParameter( const char* name, Texture* tex, int texUnit )
                    : Parameter( name ), m_texture( tex ), m_texUnit( texUnit ) {}
                virtual void bind( ShaderProgram* shader ) const override;

            private:
                Texture* m_texture;
                int m_texUnit;
            };

            template <typename T>
            class UniformBindableVector : public std::vector<T, Core::AlignedAllocator<T, 16>>
            {
            public:
                void bind( ShaderProgram* shader ) const;
            };

            typedef TParameter<int>    IntParameter;
            typedef TParameter<uint>   UIntParameter;
            typedef TParameter<Scalar> ScalarParameter;

            typedef TParameter<Core::Vector2> Vec2Parameter;
            typedef TParameter<Core::Vector3> Vec3Parameter;
            typedef TParameter<Core::Vector4> Vec4Parameter;

            typedef TParameter<Core::Matrix2> Mat2Parameter;
            typedef TParameter<Core::Matrix3> Mat3Parameter;
            typedef TParameter<Core::Matrix4> Mat4Parameter;

        public:
            void addParameter( const char* name, int    value );
            void addParameter( const char* name, uint   value );
            void addParameter( const char* name, Scalar value );

            void addParameter( const char* name, const Core::Vector2& value );
            void addParameter( const char* name, const Core::Vector3& value );
            void addParameter( const char* name, const Core::Vector4& value );

            void addParameter( const char* name, const Core::Matrix2& value );
            void addParameter( const char* name, const Core::Matrix3& value );
            void addParameter( const char* name, const Core::Matrix4& value );

            void addParameter( const char* name, Texture* tex, int texUnit );

            void bind( ShaderProgram* shader ) const;

        private:
            UniformBindableVector<IntParameter>     m_intParamsVector;
            UniformBindableVector<UIntParameter> m_uintParamsVector;
            UniformBindableVector<ScalarParameter>       m_scalarParamsVector;

            UniformBindableVector<Vec2Parameter> m_vec2ParamsVector;
            UniformBindableVector<Vec3Parameter> m_vec3ParamsVector;
            UniformBindableVector<Vec4Parameter> m_vec4ParamsVector;

            UniformBindableVector<Mat2Parameter> m_mat2ParamsVector;
            UniformBindableVector<Mat3Parameter> m_mat3ParamsVector;
            UniformBindableVector<Mat4Parameter> m_mat4ParamsVector;

            UniformBindableVector<TextureParameter> m_texParamsVector;
        };

    } // namespace Engine
} // namespace Ra

#include <Engine/Renderer/RenderTechnique/RenderParameters.inl>

#endif // RADIUMENGINE_RENDERPARAMETERS_HPP
