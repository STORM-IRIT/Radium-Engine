#pragma once
#include "DirLight.hpp"

namespace Ra {
namespace Engine {
namespace Scene {

inline void DirectionalLight::setDirection( const Eigen::Matrix<Scalar, 3, 1>& dir ) {
    m_direction = dir.normalized();
}

inline const Eigen::Matrix<Scalar, 3, 1>& DirectionalLight::getDirection() const {
    return m_direction;
}

} // namespace Scene
} // namespace Engine
} // namespace Ra
