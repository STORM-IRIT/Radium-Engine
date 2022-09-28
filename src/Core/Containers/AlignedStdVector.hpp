#pragma once
#include <Core/RaCore.hpp>

#include <Eigen/StdVector>
#include <vector>

namespace Ra {
namespace Core {

/// Shortcut for the ubiquitous aligned std::vector
/// Uses Eigen's aligned allocator, as stated in
/// http://eigen.tuxfamily.org/dox/group__TopicStlContainers.html
template <typename T>
using AlignedStdVector = std::vector<T, Eigen::aligned_allocator<T>>;
} // namespace Core
} // namespace Ra
