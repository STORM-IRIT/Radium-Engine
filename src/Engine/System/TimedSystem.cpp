#include <Engine/System/TimedSystem.hpp>

#ifdef COMPILER_MSVC
namespace Ra::Engine {
// MSVC is somehow not able to export the virtual methods implemented in BaseCouplingSystem,
// so we use this trick to force the exportation of the specialized version inherited by
// CoupledTimedSystem.
BaseCouplingSystem<AbstractTimedSystem>;
} // namespace Ra::Engine
#endif
