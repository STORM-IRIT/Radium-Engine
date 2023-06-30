#include <Core/Material/MaterialModel.hpp>
#include <Core/Utils/Log.hpp>

namespace Ra {
namespace Core {
namespace Material {
void MaterialModel::displayInfo() const {
    using namespace Core::Utils; // log
    LOG( logERROR ) << "MaterialModel : unknown material type : " << m_materialType;
}
} // namespace Material
} // namespace Core
} // namespace Ra
