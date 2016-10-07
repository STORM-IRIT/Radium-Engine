#ifndef EDGECOLLAPSE_H
#define EDGECOLLAPSE_H


#include <Core/RaCore.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>
#include <Core/Math/LinearAlgebra.hpp>

namespace Ra {
namespace Core {
namespace DcelOperations {
    RA_CORE_API void edgeCollapse( Dcel& dcel, Index edgeIndex, Vector3 p_result);
}
}
}

#endif // EDGECOLLAPSE_H
