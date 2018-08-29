#ifndef RADIUMENGINE_MAP_FILE_MANAGER_HPP
#define RADIUMENGINE_MAP_FILE_MANAGER_HPP

#include <Core/File/deprecated/FileManager.hpp>
#include <Core/Geometry/Mapping/Mapping.hpp>

namespace Ra {
namespace Core {

using namespace Geometry;

/**
 * The class MAPFileManager handles the loading/storing of MAP files.
 * These files contains a mapping in the form:
 *  \code{.unparsed}
 *       HEADER #rows
 *       ID Alpha Beta Delta
 *       ...
 *       ID Alpha Beta Delta
 *  \endcode
 *
 * where HEADER is equal to the string "MAP" and \#rows defines how many rows there are in the file.
 */
class MAPFileManager : public FileManager<Parametrization> {
  public:
    MAPFileManager();

    virtual ~MAPFileManager();

  protected:
    /// Returns the expected file header.
    std::string header() const;

    std::string fileExtension() const override;
    bool importData( std::istream& file, Parametrization& data ) override;
    bool exportData( std::ostream& file, const Parametrization& data ) override;
};

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_MAP_FILE_MANAGER_HPP
