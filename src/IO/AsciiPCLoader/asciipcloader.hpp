#pragma once

#include <Core/Asset/FileLoaderInterface.hpp>
#include <IO/RaIO.hpp>

using std::string;
using std::vector;

namespace Ra {
namespace IO {

class RA_IO_API AsciiPointCloudLoader : public Ra::Core::Asset::FileLoaderInterface
{
  public:
    AsciiPointCloudLoader();

    virtual ~AsciiPointCloudLoader();

    vector<string> getFileExtensions() const override;
    bool handleFileExtension( const std::string& extension ) const override;

    /** load vertices from .txt file
     *
     * load x, y, z and time from ASCII .txt file
     *
     * expected content in file: header line + data records
     *
     * header line content : attribute names separated by blanks ( )
     *
     * example: Time X Y Z Roll Pitch Heading sdX sdY sdZ
     * @note Although the header line can contain attributes other than coordinates and time,
     * only these are loaded. X, Y, Z coordinates search in header line is case sensitive
     * @param [in] filename file path
     * @return nullptr if file opening, geometry creation fails
     */
    Ra::Core::Asset::FileData* loadFile( const std::string& filename ) override;
    string name() const override;
};

} // namespace IO
} // namespace Ra
