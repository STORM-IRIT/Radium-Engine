#pragma once

#include <Core/CoreMacros.hpp>

/// Defines the correct macro to export dll symbols.
#if defined DummyLibrary_EXPORTS
#    define DummyLibrary_API DLL_EXPORT
#else
#    define DummyLibrary_API DLL_IMPORT
#endif

#include <string>

namespace DummyLibrary {
void DummyLibrary_API DummyMessage( const std::string& message );
} // namespace DummyLibrary
