#ifndef RADIUMENGINE_RENDERPARAMETERS_HPP
#define RADIUMENGINE_RENDERPARAMETERS_HPP

#include <Engine/RaEngine.hpp>

#include <set>
#include <vector>

#include <Core/Containers/AlignedAllocator.hpp>
#include <Core/Log/Log.hpp>
#include <Core/Math/LinearAlgebra.hpp>

namespace Ra {
namespace Engine {
class Texture;
class ShaderProgram;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Engine {
class RA_ENGINE_API RenderParameters final {
  public:
    class Parameter {
      public:
        Parameter() = default;
        explicit Parameter( const char* name ) : m_name( name ) {}
        virtual ~Parameter() = default;
        virtual void bind( const ShaderProgram* shader ) const = 0;

        const char* m_name {nullptr};
    };

    template <typename T>
    class TParameter final : public Parameter {
      public:
        TParameter() = default;
        TParameter( const char* name, const T& value ) : Parameter( name ), m_value( value ) {}
        ~TParameter() override = default;
        void bind( const ShaderProgram* shader ) const override;

        T m_value;
    };

    class TextureParameter final : public Parameter {
      public:
        TextureParameter() = default;
        TextureParameter( const char* name, Texture* tex, int texUnit ) :
            Parameter( name ),
            m_texture( tex ),
            m_texUnit( texUnit ) {}
        ~TextureParameter() override = default;
        void bind( const ShaderProgram* shader ) const override;

        Texture* m_texture {nullptr};
        int m_texUnit {-1};
    };

    template <typename T>
    class UniformBindableVector final
        : public std::map<
              std::string, T, std::less<std::string>,
              Core::AlignedAllocator<std::pair<const std::string, T>, RA_DEFAULT_ALIGN>> {
      public:
        void bind( const ShaderProgram* shader ) const;
    };

    typedef TParameter<int> IntParameter;
    typedef TParameter<uint> UIntParameter;
    typedef TParameter<Scalar> ScalarParameter;

    typedef TParameter<std::vector<int>> IntsParameter;
    typedef TParameter<std::vector<uint>> UIntsParameter;
    //! globjects seems to not handle vector of double
    typedef TParameter<std::vector<float>> ScalarsParameter;

    using Vec2Parameter = TParameter<Core::Vector2>;
    using Vec3Parameter = TParameter<Core::Vector3>;
    using Vec4Parameter = TParameter<Core::Vector4>;

    using Mat2Parameter = TParameter<Core::Matrix2>;
    using Mat3Parameter = TParameter<Core::Matrix3>;
    using Mat4Parameter = TParameter<Core::Matrix4>;

  public:
    void addParameter( const char* name, int value );
    void addParameter( const char* name, uint value );
    void addParameter( const char* name, Scalar value );

    void addParameter( const char* name, std::vector<int> values );
    void addParameter( const char* name, std::vector<uint> values );
    void addParameter( const char* name, std::vector<Scalar> values );

    void addParameter( const char* name, const Core::Vector2& value );
    void addParameter( const char* name, const Core::Vector3& value );
    void addParameter( const char* name, const Core::Vector4& value );

    void addParameter( const char* name, const Core::Matrix2& value );
    void addParameter( const char* name, const Core::Matrix3& value );
    void addParameter( const char* name, const Core::Matrix4& value );

    void addParameter( const char* name, Texture* tex, int texUnit );

    void updateParameter( const char* name, int value );
    void updateParameter( const char* name, uint value );
    void updateParameter( const char* name, Scalar value );

    void updateParameter( const char* name, const Core::Vector2& value );
    void updateParameter( const char* name, const Core::Vector3& value );
    void updateParameter( const char* name, const Core::Vector4& value );

    void updateParameter( const char* name, const Core::Matrix2& value );
    void updateParameter( const char* name, const Core::Matrix3& value );
    void updateParameter( const char* name, const Core::Matrix4& value );

    void concatParameters( const RenderParameters& params );

    void bind( const ShaderProgram* shader ) const;

    void print() const {
        for ( const auto& p : m_scalarParamsVector )
        {
            LOG( logDEBUG ) << "  " << p.first << " : " << p.second.m_name;
        }
    }

  private:
    // FIXME: Any way to simplify this a bit ?
    UniformBindableVector<IntParameter> m_intParamsVector;
    UniformBindableVector<UIntParameter> m_uintParamsVector;
    UniformBindableVector<ScalarParameter> m_scalarParamsVector;

    UniformBindableVector<IntsParameter> m_intsParamsVector;
    UniformBindableVector<UIntsParameter> m_uintsParamsVector;
    UniformBindableVector<ScalarsParameter> m_scalarsParamsVector;

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
