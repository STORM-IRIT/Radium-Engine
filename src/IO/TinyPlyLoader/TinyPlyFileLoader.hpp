#pragma once

#include <Core/Asset/FileData.hpp>
#include <Core/Asset/FileLoaderInterface.hpp>
#include <IO/RaIO.hpp>
#include <string>
#include <vector>

namespace Ra {
namespace IO {

class RA_IO_API TinyPlyFileLoader : public Core::Asset::FileLoaderInterface
{
  public:
    TinyPlyFileLoader();

    virtual ~TinyPlyFileLoader();

    std::vector<std::string> getFileExtensions() const override;
    bool handleFileExtension( const std::string& extension ) const override;
    Core::Asset::FileData* loadFile( const std::string& filename ) override;
    std::string name() const override;
};

} // namespace IO
} // namespace Ra
