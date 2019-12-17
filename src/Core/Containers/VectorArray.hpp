#ifndef RADIUMENGINE_VECTORARRAY_HPP
#define RADIUMENGINE_VECTORARRAY_HPP

#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {

/// This class is a wrapper around a std::vector of Core::Vectors.
/// which allow to use the stdlib's dynamic array implementation, yet pass it as
/// a matrix when Eigen needs it with the getMap() method.
template <typename V>
class VectorArray : public AlignedStdVector<V>
{
  public:
    // Type shortcuts
    using Vector         = V;
    using Matrix         = Eigen::Matrix<typename V::Scalar, V::RowsAtCompileTime, Eigen::Dynamic>;
    using MatrixMap      = Eigen::Map<Matrix>;
    using ConstMatrixMap = Eigen::Map<const Matrix>;

  public:
    /// Inheriting constructors from std::vector
    using AlignedStdVector<V>::AlignedStdVector;

    /// Returns the array as an Eigen Matrix Map
    MatrixMap getMap() {
        CORE_ASSERT( !this->empty(), "Cannot map an empty vector " );
        return MatrixMap( this->data()->data(), V::RowsAtCompileTime, this->size() );
    }

    /// Returns the array as an Eigen Matrix Map (const version)
    ConstMatrixMap getMap() const {
        CORE_ASSERT( !this->empty(), "Cannot map an empty vector " );
        return ConstMatrixMap( this->data()->data(), V::RowsAtCompileTime, this->size() );
    }
};

/// This specialization stores an array of scalars which can be used as a dynamic
/// Eigen column vector.
template <>
class VectorArray<Scalar> : public AlignedStdVector<Scalar>
{
  public:
    // Type shortcuts
    using Matrix         = Eigen::Matrix<Scalar, 1, Eigen::Dynamic>;
    using MatrixMap      = Eigen::Map<Matrix>;
    using ConstMatrixMap = Eigen::Map<const Matrix>;

  public:
    /// Inheriting constructors from std::vector
    using AlignedStdVector<Scalar>::AlignedStdVector;

    /// Returns the array as an Eigen Matrix Map
    MatrixMap getMap() {
        CORE_ASSERT( !this->empty(), "Cannot map an empty vector " );
        return MatrixMap( this->data(), 1, Eigen::Index( this->size() ) );
    }

    /// Returns the array as an Eigen Matrix Map (const version)
    ConstMatrixMap getMap() const {
        CORE_ASSERT( !this->empty(), "Cannot map an empty vector " );
        return ConstMatrixMap( this->data(), 1, Eigen::Index( this->size() ) );
    }
};

// Convenience aliases
using Vector1Array = VectorArray<Scalar>;
#define DEFINE_CONVENIENCE_MATRIX_ALIAS( NAME, TYPE, DIM ) \
    using NAME = VectorArray<Eigen::Matrix<TYPE, DIM, 1>>;
DEFINE_CONVENIENCE_MATRIX_ALIAS( Vector2Array, Scalar, 2 )
DEFINE_CONVENIENCE_MATRIX_ALIAS( Vector3Array, Scalar, 3 )
DEFINE_CONVENIENCE_MATRIX_ALIAS( Vector4Array, Scalar, 4 )
DEFINE_CONVENIENCE_MATRIX_ALIAS( VectorNArray, Scalar, Eigen::Dynamic )
DEFINE_CONVENIENCE_MATRIX_ALIAS( Vector1iArray, int, 1 )
DEFINE_CONVENIENCE_MATRIX_ALIAS( Vector2iArray, int, 2 )
DEFINE_CONVENIENCE_MATRIX_ALIAS( Vector3iArray, int, 3 )
DEFINE_CONVENIENCE_MATRIX_ALIAS( Vector4iArray, int, 4 )
DEFINE_CONVENIENCE_MATRIX_ALIAS( VectorNiArray, int, Eigen::Dynamic )
DEFINE_CONVENIENCE_MATRIX_ALIAS( Vector1uArray, uint, 1 )
DEFINE_CONVENIENCE_MATRIX_ALIAS( Vector2uArray, uint, 2 )
DEFINE_CONVENIENCE_MATRIX_ALIAS( Vector3uArray, uint, 3 )
DEFINE_CONVENIENCE_MATRIX_ALIAS( Vector4uArray, uint, 4 )
DEFINE_CONVENIENCE_MATRIX_ALIAS( VectorNuArray, uint, Eigen::Dynamic )
#undef DEFINE_CONVENIENCE_MATRIX_ALIAS

// Notes :
// Using a map for eigen integration was recommended by [1].
// According to this document [2], it is necessary to use a special allocator to enforce
// alignment...
// But actually it is not necessary to do so in C++11 [3] (it is actually harmful
// as it removes some vector features such as initializer lists).
// Finally we use our own aligned allocator.

// [1] https://forum.kde.org/viewtopic.php?f=74&t=126959
// [2] http://eigen.tuxfamily.org/dox-devel/group__TopicStlContainers.html
// [3] http://eigen.tuxfamily.org/bz/show_bug.cgi?id=829

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_VECTORARRAY_HPP
