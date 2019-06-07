#ifndef RADIUMENGINE_ABSTRACTGEOMETRY_HPP
#define RADIUMENGINE_ABSTRACTGEOMETRY_HPP

#include <Core/RaCore.hpp>
#include <Core/Types.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

/// Abstract class for Geometry objects in Radium Core
///
/// \warning: Part of the current proposal for a modular implementation of
/// displayable objects.
///
/// \note We use a struct because all members are public anyway
struct RA_CORE_API AbstractGeometry {
    /*
     * Note: Explicitly defaulted virtual destructor, copy/move constructors,
     * copy/move assignment operators
     * When a base class is intended for polymorphic use, its destructor may have
     * to be declared public and virtual. This blocks implicit moves
     * (and deprecates implicit copies), and so the special member functions
     * have to be declared as defaulted.
     * See http://en.cppreference.com/w/cpp/language/rule_of_three
     */
    virtual ~AbstractGeometry()                 = default;
    AbstractGeometry()                          = default;
    AbstractGeometry( const AbstractGeometry& ) = default;
    AbstractGeometry& operator=( const AbstractGeometry& ) = default;

    /// Erases all data, making the geometry empty.
    virtual void clear() = 0;

    /// Compute bounding box
    virtual Aabb computeAabb() const = 0;
};

} // namespace Geometry
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_ABSTRACTGEOMETRY_HPP
