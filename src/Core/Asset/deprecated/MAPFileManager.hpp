#ifndef RADIUMENGINE_MAP_FILE_MANAGER_HPP
#define RADIUMENGINE_MAP_FILE_MANAGER_HPP

#include <Core/Asset/deprecated/FileManager.hpp>
#include <Core/Geometry/Mapping.hpp>

namespace Ra {
namespace Core {
namespace Asset {
namespace deprecated {

using namespace Geometry;

/*
 * The class MAPFileManager handles the loading/storing of MAP files.
 * These files contains a mapping in the form:
 *
 *       HEADER #rows
 *       ID Alpha Beta Delta
 *       ...
 *       ID Alpha Beta Delta
 *
 * where HEADER is equal to the string "MAP" and #rows defines how many rows there are in the file.
 */
class MAPFileManager : public FileManager<Parametrization> {
  public:
    /// CONSTRUCTOR
    MAPFileManager();

    /// DESTRUCTOR
    virtual ~MAPFileManager();

  protected:
    /// HEADER
    std::string header() const;

    /// INTERFACE
    virtual std::string fileExtension() const override;
    virtual bool importData( std::istream& file, Parametrization& data ) override;
    virtual bool exportData( std::ostream& file, const Parametrization& data ) override;
};

} // namespace deprecated
} // namespace Asset
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_MAP_FILE_MANAGER_HPP
