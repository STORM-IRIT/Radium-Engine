#ifndef RADIUMENGINE_DATA_LOADER_HPP
#define RADIUMENGINE_DATA_LOADER_HPP

#include <memory>
#include <vector>

struct aiScene;

namespace Ra {
namespace Asset {

template < class DATA >
class DataLoader {
public:
    /// CONSTRUCTOR
    DataLoader( const bool VERBOSE_MODE = false ) : m_verbose(VERBOSE_MODE){ }

    /// DESTRUCTOR
    ~DataLoader() { }

    /// INTERFACE
    virtual void loadData( const aiScene* scene, std::vector< std::unique_ptr< DATA > >& data ) = 0;

    /// VERBOSE
    inline bool isVerbose() const {
        return m_verbose;
    }

    inline void setVerbose( const bool VERBOSE_MODE ) {
        m_verbose = VERBOSE_MODE;
    }

protected:
    bool m_verbose;
};

} // namespace Asset
} // namespace Ra

#endif // RADIUMENGINE_DATA_LOADER_HPP
