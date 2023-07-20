#pragma once
#include <IO/RaIO.hpp>

#include <IO/Gltf/internal/fx/gltf.h>

namespace Ra {
namespace IO {
namespace GLTF {

/**
 *
 */
class AccessorReader
{
  public:
    /**
     * Constructor of the accessorReader
     * @param doc the gltf's document
     */
    explicit AccessorReader( const fx::gltf::Document& doc ) : m_doc( doc ) {};

    /**
     * Destructor of the accessorReader
     */
    ~AccessorReader() {
        for ( std::pair<int32_t, uint8_t*> p : m_accessors ) {
            delete p.second;
        }
    };

    /**
     * Read the accessor
     * @param accessorIndex index of the gltf's accessor
     * @return a pointer to the data. The pointer can be cast the the corresponding type.
     * If the data should be normalized, the stored data's type is float. returns nullptr if
     * accessorIndex is invalid
     *
     */
    uint8_t* read( int32_t accessorIndex );

  private:
    const fx::gltf::Document& m_doc;
    std::map<int32_t, uint8_t*> m_accessors;
};

} // namespace GLTF
} // namespace IO
} // namespace Ra
