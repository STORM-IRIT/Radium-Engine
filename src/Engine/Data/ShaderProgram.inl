#pragma once
#include "CoreMacros.hpp"
#include <Engine/Data/ShaderProgram.hpp>

#include <globjects/Program.h>
#include <type_traits>

namespace Ra {
namespace Engine {
namespace Data {

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

template <>
RA_ENGINE_API void ShaderProgram::setUniform( const char* name, const Scalar& value ) const;

template <>
RA_ENGINE_API void ShaderProgram::setUniform( const char* name,
                                              const std::vector<Scalar>& value ) const;

// Uniform setters
template <typename T>
inline void ShaderProgram::setUniform( const char* name, const T& value ) const {
    m_program->setUniform<T>( name, value );
}

} // namespace Data
} // namespace Engine
} // namespace Ra
