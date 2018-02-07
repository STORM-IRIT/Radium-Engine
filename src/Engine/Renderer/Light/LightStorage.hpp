#ifndef RADIUMENGINE_LIGHTSTORAGE_HPP
#define RADIUMENGINE_LIGHTSTORAGE_HPP

#include <Engine/RaEngine.hpp>
#include <Engine/Renderer/Light/Light.hpp>


namespace Ra {
    namespace Engine {
        class RenderParameters;
    }
}


namespace Ra {
    namespace Engine {

        /**
         * Interface providing functions to access and store lights in
         * an implementation-defined way.
         */
        class RA_ENGINE_API LightStorage {
        public:
            /// Constructor
            LightStorage() {}

            /// Destructor
            virtual ~LightStorage() {}

            /**
             * Upload data to the GPU.
             */
            virtual void upload() const = 0;

            // Redefine container classic functions.

            /// Returns the container size.
            virtual size_t size() const = 0;

            /// Push a Light to the container.
            virtual void push(Light& li) = 0;

            /// Pop a Light from the container.
            //virtual Light& pop() = 0;

            /// Clear the container.
            virtual void clear() = 0;

            /// Access the container.
            virtual Light& operator[](unsigned int n) = 0;
        };

    }
}


#endif //RADIUMENGINE_LIGHTSTORAGE_HPP
