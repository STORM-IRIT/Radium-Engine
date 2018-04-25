# Radium Engine default rendering informations
**TODO : update this documentation wrt the new rendering algorithm**

_See [Material management in the Radium Engine](./material.md) documentation for understanding what is a render technique 
and how it interoperates with geometry, material and shaders._

## Main render method

The `render` method cannot be modified, it always does the following stuff 

```
void Renderer::render( const RenderData& data )
{
  // 0. Save an eventual already bound FBO
  saveExternalFBOInternal();

  // 1. Gather render objects and update them
  renderObjects = m_engine->getRenderObjectManager()->getRenderObjects();
  updateRenderObjectsInternal( data, renderObjects );

  // 2. Feed render queues
  feedRenderQueuesInternal( data, renderObjects );

  // 3. Do picking if needed
  if ( !m_pickingQueries.empty() )
  {
    doPicking();
  }

  // 4. Do the rendering.
  renderInternal( data );

  // 5. Post processing
  postProcessInternal( data );

  // 6. Write final texture to framebuffer / backbuffer
  // (depending on wether anything was saved on step 0. or not)
  drawScreenInternal();
}
```

### 0. Save an eventual already bound FBO
In some cases (like with `QtOpenGLWidget`), you do not draw directly on the screen, but you have instead
to feed an already bound FBO. Since the default renderer uses multiple FBOs, the Qt's one must be saved. 

*This behaviour cannot be modified.*

### 3. Do picking if needed
If there has been some picking requests since the last frame, `doPicking` is called.
This function just renders all the objects (except *debug* ones) by drawing them in some color given the ID 
of the entity a render object is attached to.

Then, for each picking request done, `glReadPixels` is called at the requested location, and object ID is retrieved.

### 4. Do the rendering
This method does most of the whole rendering work 
and outputs one final *renderpass* texture, ready to be postprocessed.

Here is a summary of all the draw calls
#### 1. Depth, ambient color and "deferred info" pass
This step 
  * only concerns opaque objects. It is used to, first, optimize the next pass by not drawing hidden fragments 
(early z-test since the depth buffer is already filled).
  * saves ambient color for each object.
  * saves the world normals and world positions for each object.
  
#### 2. Lighing pass
This pass is a classic forward lighting pass. Blending (one, one) is enabled and it does 
```
for each light do
  for each shader do
    bind shader
    bind light
    for each material do
      bind material
      for each mesh do
        render mesh
      done
    done
  done
done
```

#### 3. Debug and UI render objects
Those objects are drawn with their own shader, and without lighting.
They do not write in the depth map but they have different depth testing behaviour :
  * func is LESS for debug objects (drawn only if visibles)
  * func is ALWAYS for UI objects (drawn in front of everything else)

#### 4. Ordered independent transparency pass
This pass basically implements the first pass of NVidia paper on Weighted blended order independent transparency
(see http://jcgt.org/published/0002/02/09/)

#### 5. Compositing for the whole *render pass*
Two draw calls are done for this pass, 
  * the first one composes ambient and lighted textures
  * the second one blends (1-alpha, alpha) the last render result with the transparency textures

### 5. Do post-processing
For now, this method just copies `renderpass` texture to `final` texture.

### 6. Write final texture to framebuffer / backbuffer
This method is just responsible for displaying the final stuff on screen or on the saved FBO.

## General informations
  * Only two methods can be overrided for the renderer, renderInternal (step 4) and postProcessInternal (step 5).
  
## TODO
  * Ambient occlusion
  * Shadow mapping
  * Skybox 
  * Reflection / refraction
  * Tonemapping
  * Bloom
  * Motion blur
  * FOV
  * Physically based rendering
