#ifndef RADIUMENGINE_VECTORARRAY_HPP
#define RADIUMENGINE_VECTORARRAY_HPP

#include <vector>

#include <Core/Math/Vector.hpp>

namespace Ra { namespace Core
{
    /// This class is a wrapper around a std::vector of Core::Vectors (aka Eigen::Vector4)
    /// which allow to use the stdlib's dynamic array implementation, yet pass it as 
    /// a matrix when Eigen needs it with the getMap() method.

    // see https://forum.kde.org/viewtopic.php?f=74&t=126959

    template< typename V >
    class VectorArray : public std::vector<V>
    {
    public:
        // Type shortcuts
        typedef Eigen::Matrix<Scalar, V::RowsAtCompileTime, Eigen::Dynamic> Matrix;
        typedef Eigen::Map<Matrix, Eigen::Aligned> MatrixMap;
        typedef Eigen::Map<const Matrix, Eigen::Aligned> ConstMatrixMap;

    public:
        /// Inheriting constructors from std::vector
        using std::vector<V>::vector;

        /// Returns the array as an Eigen Matrix Map
        MatrixMap getMap()
        {
            return MatrixMap(this->data()->data(), V::RowsAtCompileTime, this->size());
        }

        /// Returns the array as an Eigen Matrix Map (const version)
        ConstMatrixMap getMap() const
        {
            return ConstMatrixMap(this->data()->data(), V::RowsAtCompileTime, this->size());
        }

    };
}}


#endif //RADIUMENGINE_VECTORARRAY_HPP
