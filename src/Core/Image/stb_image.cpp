// Implement stb_image here

#include <Core/RaCore.hpp>
#define STBI_ASSERT( X ) CORE_ASSERT( X, "STB image assert" );

#define STB_IMAGE_IMPLEMENTATION
#include <Core/Image/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <Core/Image/stb_image_write.h>
