#include <Core/Containers/VariableSet.hpp>
#include <Core/Containers/VariableSetEnumManagement.hpp>
#include <Core/Utils/Log.hpp>
#include <functional>
#include <map>
#include <ostream>
#include <string>
#include <utility>

namespace Ra {
namespace Core {
namespace VariableSetEnumManagement {

void setEnumVariable( VariableSet& vs, const std::string& name, const std::string& value ) {
    auto converterFunc = vs.existsVariable<
        std::function<void( Core::VariableSet&, const std::string&, const std::string& )>>( name );
    if ( converterFunc ) { ( *converterFunc )->second( vs, name, value ); }
    else {
        LOG( Core::Utils::logWARNING ) << "VariableSet: try to set enum value from string without "
                                          "converter, ignored. Variable name: ["
                                       << name << "], value: [" << value << "]";
    }
}

void setEnumVariable( VariableSet& vs, const std::string& name, const char* value ) {
    setEnumVariable( vs, name, std::string( value ) );
}

} // namespace VariableSetEnumManagement
} // namespace Core
} // namespace Ra
