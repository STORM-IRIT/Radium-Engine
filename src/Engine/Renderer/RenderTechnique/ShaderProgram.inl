#include <globjects/Program.h>

namespace Ra {
namespace Engine {

// declare specialization, definied in .cpp
template <>
RA_ENGINE_API void ShaderProgram::setUniform( const char* name, const Core::Vector2d& value ) const;

template <>
RA_ENGINE_API void ShaderProgram::setUniform( const char* name, const Core::Vector3d& value ) const;

template <>
RA_ENGINE_API void ShaderProgram::setUniform( const char* name, const Core::Vector4d& value ) const;

template <>
RA_ENGINE_API void ShaderProgram::setUniform( const char* name, const Core::Matrix2d& value ) const;

template <>
RA_ENGINE_API void ShaderProgram::setUniform( const char* name, const Core::Matrix3d& value ) const;

template <>
RA_ENGINE_API void ShaderProgram::setUniform( const char* name, const Core::Matrix4d& value ) const;

// Uniform setters
template <typename T>
inline void ShaderProgram::setUniform( const char* name, const T& value ) const {
    m_program->setUniform<T>( name, value );
}

} // namespace Engine
} // namespace Ra
