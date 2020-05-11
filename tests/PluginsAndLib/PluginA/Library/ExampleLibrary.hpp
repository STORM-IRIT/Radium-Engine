#pragma once

#include <Core/CoreMacros.hpp>

/// Defines the correct macro to export dll symbols.
#if defined ExampleLibrary_EXPORTS
#    define ExampleLibrary_API DLL_EXPORT
#else
#    define ExampleLibrary_API DLL_IMPORT
#endif

#include <string>

namespace ExampleLibrary {
void ExampleLibrary_API ExampleMessage( const std::string& message );
} // namespace ExampleLibrary
