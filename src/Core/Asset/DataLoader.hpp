#ifndef RADIUMENGINE_DATA_LOADER_HPP
#define RADIUMENGINE_DATA_LOADER_HPP

#include <memory>
#include <vector>

struct aiScene;

namespace Ra {
namespace Core {
namespace Asset {

/**
 * The DataLoader class is a generic data converter from Assimp scenes.
 */
// FIXME: this should go to IO/AssipLoader
template <class DATA>
class DataLoader {
  public:
    explicit DataLoader( const bool VERBOSE_MODE = false ) : m_verbose( VERBOSE_MODE ) {}

    virtual ~DataLoader() = default;

    /**
     * Converts the Assimp scene into DATA.
     */
    virtual void loadData( const aiScene* scene, std::vector<std::unique_ptr<DATA>>& data ) = 0;

    /**
     * Return true if the DataLoader is verbose.
     */
    inline bool isVerbose() const { return m_verbose; }

    /**
     * Toggle on/off verbosity.
     */
    inline void setVerbose( const bool VERBOSE_MODE ) { m_verbose = VERBOSE_MODE; }

  protected:
    /// Whether the DataLoader is verbose.
    bool m_verbose;
};

} // namespace Asset
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_DATA_LOADER_HPP
