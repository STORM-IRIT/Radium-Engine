\page develLights API: Lights
[TOC]

- Lights are components associated with an entity.
- Lights can be attached to any of the entities of a given scene.
- Lights are loaded by a LightManager system. If no system is able to load lights, there will be no lights in the scene
except to one that can be attached to the camera.
- The way lights are used in the engine is specific to the renderer.
- A renderer must, in its initializeInternal method, create and register a lightManager system.
- The system will receive fileData at loading and will be polled by the renderer to get light to render.

## Light Manager

## Loading lights
Light are loaded by the active file loader and transformed to a LightData vector in th FileData constructed by the loader.

## Using light
