#include <Core/Utils/Log.hpp>
#include <Engine/Data/RenderParameters.hpp>
#include <Engine/Data/ShaderProgram.hpp>

namespace Ra {
namespace Engine {
namespace Data {
void RenderParameters::bind( const Data::ShaderProgram* shader ) const {
    m_boolParamsVector.bind( shader );
    m_intParamsVector.bind( shader );
    m_uintParamsVector.bind( shader );
    m_scalarParamsVector.bind( shader );
    m_intsParamsVector.bind( shader );
    m_uintsParamsVector.bind( shader );
    m_scalarsParamsVector.bind( shader );
    m_vec2ParamsVector.bind( shader );
    m_vec3ParamsVector.bind( shader );
    m_vec4ParamsVector.bind( shader );
    m_colorParamsVector.bind( shader );
    m_mat2ParamsVector.bind( shader );
    m_mat3ParamsVector.bind( shader );
    m_mat4ParamsVector.bind( shader );
    m_texParamsVector.bind( shader );
}

void RenderParameters::addEnumConverter( const std::string& name,
                                         const std::map<std::string, int>& converter ) {
    m_enumConverters[name] = converter;
}

bool RenderParameters::containsEnumConverter( const std::string& name ) {
    return m_enumConverters.find( name ) != m_enumConverters.end();
}

void RenderParameters::addParameter( const std::string& name, bool value ) {
    m_boolParamsVector[name] = BoolParameter( name, value );
}

void RenderParameters::addParameter( const std::string& name, int value ) {
    m_intParamsVector[name] = IntParameter( name, value );
}

void RenderParameters::addParameter( const std::string& name, uint value ) {
    m_uintParamsVector[name] = UIntParameter( name, value );
}

void RenderParameters::addParameter( const std::string& name, Scalar value ) {
    m_scalarParamsVector[name] = ScalarParameter( name, value );
}

///!! array version

void RenderParameters::addParameter( const std::string& name, const std::vector<int>& value ) {
    m_intsParamsVector[name] = IntsParameter( name, value );
}

void RenderParameters::addParameter( const std::string& name, const std::vector<uint>& value ) {
    m_uintsParamsVector[name] = UIntsParameter( name, value );
}

void RenderParameters::addParameter( const std::string& name, const std::vector<Scalar>& value ) {
    m_scalarsParamsVector[name] = ScalarsParameter( name, value );
}

///!!

void RenderParameters::addParameter( const std::string& name, const Core::Vector2& value ) {
    m_vec2ParamsVector[name] = Vec2Parameter( name, value );
}

void RenderParameters::addParameter( const std::string& name, const Core::Vector3& value ) {
    m_vec3ParamsVector[name] = Vec3Parameter( name, value );
}

void RenderParameters::addParameter( const std::string& name, const Core::Vector4& value ) {
    m_vec4ParamsVector[name] = Vec4Parameter( name, value );
}

void RenderParameters::addParameter( const std::string& name, const Core::Utils::Color& value ) {
    m_colorParamsVector[name] = ColorParameter( name, value );
}

void RenderParameters::addParameter( const std::string& name, const Core::Matrix2& value ) {
    m_mat2ParamsVector[name] = Mat2Parameter( name, value );
}

void RenderParameters::addParameter( const std::string& name, const Core::Matrix3& value ) {
    m_mat3ParamsVector[name] = Mat3Parameter( name, value );
}

void RenderParameters::addParameter( const std::string& name, const Core::Matrix4& value ) {
    m_mat4ParamsVector[name] = Mat4Parameter( name, value );
}

void RenderParameters::addParameter( const std::string& name, Data::Texture* tex, int texUnit ) {
    m_texParamsVector[name] = TextureParameter( name, tex, texUnit );
}

void RenderParameters::addParameter(
    const std::string& name,
    const std::pair<const std::string&, const std::string&> enumTypeAndValue ) {

    auto converter = m_enumConverters.find( enumTypeAndValue.first );
    if ( converter != m_enumConverters.end() ) {
        auto value = converter->second.find( enumTypeAndValue.second );
        if ( value != converter->second.end() )
            addParameter( name, value->second );
        else
            LOG( Core::Utils::logERROR )
                << "RenderParameters, value not found in converter " << enumTypeAndValue.first
                << " " << enumTypeAndValue.second;
    }
    else {
        LOG( Core::Utils::logERROR ) << "RenderParameters, try to set enum value without converter "
                                     << enumTypeAndValue.first << " " << enumTypeAndValue.second;
    }
}

// apply P_FUNC to each m_*ParamsVector
#define PARAM_FUNC_HELPER           \
    P_FUNC( m_boolParamsVector )    \
    P_FUNC( m_intParamsVector )     \
    P_FUNC( m_uintParamsVector )    \
    P_FUNC( m_scalarParamsVector )  \
    P_FUNC( m_intsParamsVector )    \
    P_FUNC( m_uintsParamsVector )   \
    P_FUNC( m_scalarsParamsVector ) \
    P_FUNC( m_vec2ParamsVector )    \
    P_FUNC( m_vec3ParamsVector )    \
    P_FUNC( m_vec4ParamsVector )    \
    P_FUNC( m_colorParamsVector )   \
    P_FUNC( m_mat2ParamsVector )    \
    P_FUNC( m_mat3ParamsVector )    \
    P_FUNC( m_mat4ParamsVector )    \
    P_FUNC( m_texParamsVector )

// for concat -> insert, keep old value
#define P_FUNC( x )                        \
    for ( const auto& param : params.x ) { \
        x.insert( param );                 \
    }

void RenderParameters::mergeKeepParameters( const RenderParameters& params ) {
    PARAM_FUNC_HELPER
}
#undef P_FUNC

// for copy -> [], replace old value
#define P_FUNC( x )                        \
    for ( const auto& param : params.x ) { \
        x[param.first] = param.second;     \
    }

void RenderParameters::mergeReplaceParameters( const RenderParameters& params ) {
    PARAM_FUNC_HELPER
}
#undef P_FUNC
#undef PARAM_FUNC_HELPER

} // namespace Data
} // namespace Engine
} // namespace Ra
