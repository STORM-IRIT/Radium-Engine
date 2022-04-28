#include <Core/Asset/ImageManager.hpp>

namespace Ra {
namespace Core {
namespace Asset {

std::map<std::string, std::shared_ptr<Image>> ImageManager::s_name2image;

} // namespace Asset
} // namespace Core
} // namespace Ra
