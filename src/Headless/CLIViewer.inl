#include <Headless/CLIViewer.hpp>

namespace Ra {
namespace Headless {

/**
 * Get the filename given using the option --file <filename> or -f <filename>
 */
inline std::string CLIViewer::getDataFileName() const {
    return m_parameters.m_dataFile;
}

/**
 * Set the filename given using the option --file <filename> or -f <filename>
 */
inline void CLIViewer::setDataFileName( std::string filename ) {
    m_parameters.m_dataFile = std::move( filename );
}

inline OpenGLContext& CLIViewer::getWindow() {
    return *m_glContext;
}
} // namespace Headless
} // namespace Ra
