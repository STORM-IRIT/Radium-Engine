#ifndef RADIUMENGINE_SPLINE_HPP__
#define RADIUMENGINE_SPLINE_HPP__

#include <Core/Container/VectorArray.hpp>
#include <Core/RaCore.hpp>
#include <vector>

namespace Ra {
namespace Core {
namespace Math {


/**
 * @class Spline
 *
 * @brief Handling spline curves of arbitrary dimensions
 * @note This class use the efficient blossom algorithm to compute a position on
 * the curve.
 * @tparam D : dimension of the curve.
 * @tparam K  :order of the curve (min 2)
 */
template <uint D, uint K = 2>
class Spline {
  public:
    enum Type {
        UNIFORM,
        OPEN_UNIFORM ///< Connected to the first and last control points
    };

    using Vector = typename Eigen::Matrix<Scalar, D, 1>;

  public:
    /// Type of the nodal vector
    /// @param k : order of the spline (minimum is two)
    /// @param node_type : nodal vector type (uniform, open_uniform)
    /// This will define the behavior of the spline with its control points
    /// as well as its speed according to its parameter.
    inline Spline( Type type = OPEN_UNIFORM );

    /// Set the position of the spline control points.
    inline void setCtrlPoints( const Core::Container::VectorArray<Vector>& points );

    /// Get the control points of the spline
    inline const Core::Container::VectorArray<Vector>& getCtrlPoints() const;

    /// The the nodal vector type
    inline void setType( Type type );

    /// Evaluate position of the spline
    /// @param u : curve parameter ranging from [0; 1]
    inline Vector f( Scalar u ) const;

    /// Evaluate speed of the spline
    inline Vector df( Scalar u ) const;

  private:
    // -------------------------------------------------------------------------
    /// @name Class tools
    // -------------------------------------------------------------------------

    inline void assertSplines() const;

    /// set value and size of the nodal vector depending on the current number
    /// of control points
    inline void setNodalVector();

    /// Set values of the nodal vector to be uniform
    inline void setNodeToUniform();

    /// Set values of the nodal vector to be open uniform
    inline void setNodeToOpenUniform();

    /// Evaluate the equation of a splines using the blossom algorithm
    /// @param u : the curve parameter which range from the values
    /// [node[k-1]; node[point.size()]]
    /// @param point : the control points which size must be at least equal to
    /// the order of the spline (point.size() >= k)
    /// @param k : the spline order (degree == k-1)
    /// @param node : the nodal vector which defines the speed of the spline
    /// parameter u. The nodal vector size must be equal to (k + point.size())
    /// @param off : offset to apply to the nodal vector 'node' before reading
    /// from it. this is useful to compute derivatives.
    static inline Vector eval( Scalar u, const Core::Container::VectorArray<Vector>& points,
                               const std::vector<Scalar>& node, uint k, int off = 0 );

    static inline Vector evalRec( Scalar u, const Core::Container::VectorArray<Vector>& points,
                                  const std::vector<Scalar>& node, uint k );

    // -------------------------------------------------------------------------
    /// @name attributes
    // -------------------------------------------------------------------------

    Core::Container::VectorArray<Vector> m_points; ///< Control points
    Core::Container::VectorArray<Vector> m_vecs;   ///< Control points differences
    std::vector<Scalar> m_node;         ///< Nodal vector
    Type m_type;                        ///< Nodal vector type
};

} // namespace Math
} // namespace Core
} // namespace Ra

#include <Core/Math/Spline.inl>

#endif
