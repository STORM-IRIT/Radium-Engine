#ifndef DELTA_DEFINITION
#define DELTA_DEFINITION

#include <cstdint> // uint8_t
#include <vector>

#include <Core/Math/LinearAlgebra.hpp> // Sparse
#include <Core/RaCore.hpp>             // Scalar

namespace Ra {
namespace Core {
namespace Algorithm {

// Defining a vector containing true for the indices different than 0
using BitSet = std::vector<bool>;

// Defining a vector containing the indices of the sources
using Source = std::vector<uint8_t>;

// Defining the vector containing the value of the sources
using Delta = Ra::Core::Sparse;

/*
 * Return the Delta vector.
 * The delta is defined as:
 *       delta( i ) = default_value  ,  if source( i ) is true
 *       delta( i ) = 0              ,  otherwise
 *
 * The definition was taken from:
 * "Geodesics in Heat: A New Approach to Computing Distance Based on Heat Flow"
 * [Keenan Crane, Clarisse Weischedel, Max Wardetzky ]
 * TOG 2013
 */
Delta RA_CORE_API delta( const BitSet& source, const Scalar& default_value = 1.0 );

/*
 * Return the Delta vector.
 * The delta is defined as:
 *       delta( i ) = default_value  ,  if source( i ) is true
 *       delta( i ) = 0              ,  otherwise
 *
 * The definition was taken from:
 * "Geodesics in Heat: A New Approach to Computing Distance Based on Heat Flow"
 * [Keenan Crane, Clarisse Weischedel, Max Wardetzky ]
 * TOG 2013
 */
void RA_CORE_API delta( const BitSet& source, Delta& u, const Scalar& default_value = 1.0 );

/*
 * Return the Delta vector.
 * The delta is defined as:
 *       delta( i ) = default_value  ,  if i is in source
 *       delta( i ) = 0              ,  otherwise
 *
 * The definition was taken from:
 * "Geodesics in Heat: A New Approach to Computing Distance Based on Heat Flow"
 * [Keenan Crane, Clarisse Weischedel, Max Wardetzky ]
 * TOG 2013
 */
Delta RA_CORE_API delta( const Source& source, const uint size, const Scalar& default_value = 1.0 );

/*
 * Return the Delta vector.
 * The delta is defined as:
 *       delta( i ) = default_value  ,  if i is in source
 *       delta( i ) = 0              ,  otherwise
 *
 * The definition was taken from:
 * "Geodesics in Heat: A New Approach to Computing Distance Based on Heat Flow"
 * [Keenan Crane, Clarisse Weischedel, Max Wardetzky ]
 * TOG 2013
 */
void RA_CORE_API delta( const Source& source, const uint size, Delta& u,
                        const Scalar& default_value = 1.0 );

/*
 * Return a BitSet from a given Delta.
 * The BitSet is defined as:
 *       bit_i = false   , if delta( i )  = 0
 *       bit_i = true    , if delta( i ) != 0
 */
void RA_CORE_API bitset( const Delta& u, BitSet& bit );

} // namespace Algorithm
} // namespace Core
} // namespace Ra

#endif // DELTA_DEFINITION
