#ifndef EDGECOLLAPSE_H
#define EDGECOLLAPSE_H


#include <Core/RaCore.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>

namespace Ra {
namespace Core {
namespace DcelOperations {
    RA_CORE_API void edgeCollapse( Dcel& dcel, Index edgeIndex /*, Vector3d v*/ );
}
}
}

#endif // EDGECOLLAPSE_H
