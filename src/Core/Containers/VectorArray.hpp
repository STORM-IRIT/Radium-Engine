#pragma once

#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/RaCore.hpp>

#include <type_traits> // std::is_integral

namespace Ra {
namespace Core {

/// Internal class used to compute types and values used by VectorArray, to handle both
/// eigen matrices and integral types
template <typename V, bool isIntegral>
struct VectorArrayTypeHelper {};

/// This class is a wrapper around a std::vector of Core::Vectors.
/// which allow to use the stdlib's dynamic array implementation, yet pass it as
/// a matrix when Eigen needs it with the getMap() method.
template <typename V>
class VectorArray : public AlignedStdVector<V>
{
  private:
    using TypeHelper =
        VectorArrayTypeHelper<V, std::is_integral<V>::value || std::is_floating_point<V>::value>;

  public:
    // Type shortcuts
    static constexpr int ElementSize = TypeHelper::Size;
    using Scalar                     = typename TypeHelper::Scalar;
    using Vector                     = V;
    using Matrix                     = Eigen::Matrix<Scalar, TypeHelper::Size, Eigen::Dynamic>;
    using MatrixMap                  = Eigen::Map<Matrix>;
    using ConstMatrixMap             = Eigen::Map<const Matrix>;

  public:
    /// Inheriting constructors from std::vector
    using AlignedStdVector<V>::AlignedStdVector;

    /// Returns the array as an Eigen Matrix Map
    MatrixMap getMap() {
        CORE_ASSERT( !this->empty(), "Cannot map an empty vector " );
        return MatrixMap(
            TypeHelper::getData( this ), TypeHelper::Size, Eigen::Index( this->size() ) );
    }

    /// Returns the array as an Eigen Matrix Map (const version)
    ConstMatrixMap getMap() const {
        CORE_ASSERT( !this->empty(), "Cannot map an empty vector " );
        return MatrixMap(
            TypeHelper::getConstData( this ), TypeHelper::Size, Eigen::Index( this->size() ) );
    }
};
template <typename V>
struct VectorArrayTypeHelper<V, true> {
    using Scalar              = V;
    static constexpr int Size = 1;
    static inline Scalar* getData( VectorArray<V>* v ) { return v->data(); }
    static inline Scalar* getConstData( const VectorArray<V>* v ) { return v->data(); }
};
template <typename V>
struct VectorArrayTypeHelper<V, false> {
    using Scalar              = typename V::Scalar;
    static constexpr int Size = V::RowsAtCompileTime;
    static inline Scalar* getData( VectorArray<V>* v ) { return v->data()->data(); }
    static inline Scalar* getConstData( const VectorArray<V>* v ) { return v->data()->data(); }
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
