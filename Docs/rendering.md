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

### 1. Gather render objects and update them
This construct the set of objects that must be drawn for the current frame and update their opengl state

### 2. Feed the render queue
This construct the set of render actions that must be done for the current frame.

### 3. Do picking if needed
If there has been some picking requests since the last frame, `doPicking` is called.
This function just renders all the objects (except *debug* ones) by drawing them in some color given the ID 
of the entity a render object is attached to.

Then, for each picking request done, `glReadPixels` is called at the requested location, and object ID is retrieved.

### 4. Do the rendering
This method does most of the whole rendering work 
and outputs one final *renderpass* texture, ready to be postprocessed.

The main renderer of Radium, implemented in the class ``ForwardRenderer`` implements a Z-prepass forward rendering loop.
Even if the material association to a renderable object, realized by the so called ``RenderTechnique`` is tightly 
coupled with the main rendering loop, Plugins might define new renderers and interact differently with shaders and 
materials properties. See the material chapter of the documentation.

Here is a summary of all the draw calls
#### 1. Depth, ambient color and "deferred info" pass
This pass 
*   only concerns opaque objects AND opaque fragment on transparent objects. It is used to, mainly to fill in the  
Z-buffer, allowing to activate early z-test for next passes since the depth buffer is already filled,

*   initialize the color buffer by computing e.g. the ambient color for each object,

*   generate several pictures of the scene allowing to implement compositing effects later :
    *   saves the world-space normals for each object,
    *   saves the "Diffuse" aspect of the object,
    *   saves the "Specular" aspect of the object.
 
In this pass, each ``RenderObject``is drawn with the ``RenderTechnique::Z_PREPASS`` argument so that the corresponding
shader will be activated before draw call. (``ro->render( params, renderData, RenderTechnique::Z_PREPASS );``)
  
Note that the  shader associated to  the ``RenderTechnique::Z_PREPASS`` pass must draw only fully opaque fragments. 
Fully transparent ones (rejected by a masking information such as mask texture) and blendable ones 
(those with an opacity factor alpha les than one) must be discarded.
  
#### 2. Lighting pass
This pass is a classic forward lighting pass that accumulates the color of each light source. 
Before this pass, blending (one, one) is enabled and it does 
```
for each light do
  Get light parameters
  for each object
    bind shader
    bind material & light
    render mesh
  done
done
```

In this pass, each ``RenderObject``is drawn with the ``RenderTechnique::LIGHTING_OPAQUE`` argument so that the 
corresponding shader will be activated before draw call. 
(``ro->render( params, renderData, RenderTechnique::LIGHTING_OPAQUE );``)
  
Note that the  shader associated to  the ``RenderTechnique::LIGHTING_OPAQUE`` pass must lit and draw only fully opaque 
fragments. 
Fully transparent ones (rejected by a masking information such as mask texture) and blendable ones 
(those with an opacity factor alpha les than one) must be discarded.

#### 3. Ordered independent transparency 
Rendering transparent objects in Radium is done according to the algorithm described in 
*   Weighted Blended Order-Independent Transparency,
    Morgan McGuire, Louis Bavoil - NVIDIA,
    Journal of Computer Graphics Techniques (JCGT), vol. 2, no. 2, 122-141, 2013,
    <http://jcgt.org/published/0002/02/09/>
This pass contains one scene rendering pass and one compositing pass.

*   The scene rendering pass must compute both the accumulation buffer and the coverage buffer as described in the paper  
(see the Material documentaiton for example of shaders.). It is realized the same way than the lighting pass but only 
fragments that are transparents must be lit and drawn.

*   The compositing pass then adds to the color buffer the resulting blended color.

#### 4. Post-process the whole _render pass_
This pass takes the color buffer, representing colors in linear RGB space) and apply gamma correction to the image

#### 5. Debug and UI render objects
Those objects are drawn with their own shader, and without lighting.
They do not write in the depth map but they have different depth testing behaviour :
*   func is LESS for debug objects (drawn only if visibles)
*   func is ALWAYS for UI objects (drawn in front of everything else)

### 6. Write final texture to framebuffer / backbuffer
This method is just responsible for displaying the final stuff on screen or on the saved FBO.

## General informations
  * Only two methods can be overrided for the renderer, renderInternal (step 4) and postProcessInternal (step 5).
  
## TODO
*   Ambient occlusion
*   Shadow mapping
*   Skybox 
*   Reflection / refraction
*   Tonemapping (only gamma correction is applied for now)
*   Bloom
*   Motion blur
*   FOV
*   Physically based rendering
