#include <Engine/System/TimedSystem.hpp>


#ifdef COMPILER_MSVC
namespace Ra::Engine {
    // MSVC is somehow not able to export the virtual methods implemented in BaseCouplingSystem,
    // so we use this trick to force the exportation of the specialized version inherited by
    // CoupledTimedSystem.
    /* MSVC generates the following warning on this line :
     * c:\projects\radium-engine\src\engine\system\timedsystem.cpp(9): warning C4091: '': ignored on left of
     * 'Ra::Engine::BaseCouplingSystem<Ra::Engine::AbstractTimedSystem>' when no variable is declared
     * [C:\projects\Radium-Engine\build\src\radiumEngine.vcxproj]
     */
    BaseCouplingSystem<AbstractTimedSystem>;
}
#endif
