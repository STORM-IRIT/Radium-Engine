#ifndef RADIUMENGINE_VECTORARRAY_HPP
#define RADIUMENGINE_VECTORARRAY_HPP

#include <Core/RaCore.hpp>
#include <vector>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Containers/AlignedStdVector.hpp>

namespace Ra
{
    namespace Core
    {

        /// This class is a wrapper around a std::vector of Core::Vectors.
        /// which allow to use the stdlib's dynamic array implementation, yet pass it as
        /// a matrix when Eigen needs it with the getMap() method.
        template<typename V>
        class VectorArray : public AlignedStdVector<V>
        {
        public:
            // Type shortcuts
            typedef V Vector;
            typedef Eigen::Matrix<Scalar, V::RowsAtCompileTime, Eigen::Dynamic> Matrix;
            typedef Eigen::Map<Matrix> MatrixMap;
            typedef Eigen::Map<const Matrix> ConstMatrixMap;

        public:
            /// Inheriting constructors from std::vector
            using AlignedStdVector<V>::AlignedStdVector;

            /// Returns the array as an Eigen Matrix Map
            MatrixMap getMap()
            {
                CORE_ASSERT(!this->empty(), "Cannot map an empty vector ");
                return MatrixMap(this->data()->data(), V::RowsAtCompileTime, this->size());
            }

            /// Returns the array as an Eigen Matrix Map (const version)
            ConstMatrixMap getMap() const
            {
                CORE_ASSERT(! this->empty(), "Cannot map an empty vector ");
                return ConstMatrixMap(this->data()->data(), V::RowsAtCompileTime, this->size());
            }

        };

        /// This specialization stores an array of scalars which can be used as a dynamic
        /// Eigen column vector.
        template<>
        class VectorArray<Scalar> : public AlignedStdVector<Scalar>
        {
        public:
            // Type shortcuts
            typedef Eigen::Matrix<Scalar, 1, Eigen::Dynamic> Matrix;
            typedef Eigen::Map<Matrix> MatrixMap;
            typedef Eigen::Map<const Matrix> ConstMatrixMap;

        public:
            /// Inheriting constructors from std::vector
            using AlignedStdVector<Scalar>::AlignedStdVector;

            /// Returns the array as an Eigen Matrix Map
            MatrixMap getMap()
            {
                CORE_ASSERT(!this->empty(), "Cannot map an empty vector ");
                return MatrixMap(this->data(), 1, this->size());
            }

            /// Returns the array as an Eigen Matrix Map (const version)
            ConstMatrixMap getMap() const
            {
                CORE_ASSERT(!this->empty(), "Cannot map an empty vector ");
                return ConstMatrixMap(this->data(), 1, this->size());
            }

        };

        // Convenience typedefs
        typedef VectorArray<Scalar>    Vector1Array;
        typedef VectorArray<Vector2>   Vector2Array;
        typedef VectorArray<Vector3>   Vector3Array;
        typedef VectorArray<Vector3ui> Vector3uiArray;
        typedef VectorArray<Vector4>   Vector4Array;

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

    }
}

#endif //RADIUMENGINE_VECTORARRAY_HPP
