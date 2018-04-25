#ifndef LAPLACIAN_SMOOTHING
#define LAPLACIAN_SMOOTHING

#include <Core/Geometry/Diffusion.hpp> // ScalarValue
#include <Core/Container/VectorArray.hpp>        // VectorArray
#include <Core/Geometry/Laplacian.hpp>  // LaplacianMatrix
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

/*
 * Return the clamped version of the value vector given in input.
 * The values will be clamped to 0 if one of the following happen:
 *       - v_i < 0
 *       - ( bit_i || flag ) == false
 */
RA_CORE_API ScalarValue smartClamp( const BitSet& bit, const ScalarValue& value, const bool flag );

/*
 * Return the clamped version of the value vector given in input.
 * The values will be clamped to 0 if one of the following happen:
 *       - v_i < 0
 *       - ( bit_i || flag ) == false
 */
RA_CORE_API void smartClamp( const BitSet& bit, const ScalarValue& value, ScalarValue& clamped,
                             const bool flag );

/*
 * Return the new position of the vertices v_i given the LaplacianMatrix and a set of weight, after
 * a user-defined number of iterations. Within each iteration, the new position is evaluated as the
 * weighted sum of the iteration i and iteration i-1.
 */
RA_CORE_API Container::VectorArray<Math::Vector3> laplacianSmoothing( const Container::VectorArray<Math::Vector3>& v,
                                                     const LaplacianMatrix& L,
                                                     const ScalarValue& weight,
                                                     const uint iteration );

/*
 * Return the new position of the vertices v_i given the LaplacianMatrix and a set of weight, after
 * a user-defined number of iterations. Within each iteration, the new position is evaluated as the
 * weighted sum of the iteration i and iteration i-1.
 */
RA_CORE_API void laplacianSmoothing( const Container::VectorArray<Math::Vector3>& v,
                                     const LaplacianMatrix& L,
                                     const ScalarValue& weight, const uint iteration,
                                     Container::VectorArray<Math::Vector3>& p );

} // namespace Geometry
} // namespace Core
} // namespace Ra

#endif // LAPLACIAN_SMOOTHING
