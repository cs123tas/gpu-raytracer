# GPU Raytracing:

## Idea: 
Use the graphics card for doing our ray tracing computations in parallel.

### How:
We can render our scene to a textured fullscreen quad, doing all the compute in a fragment or compute shader.  In this way, we cast a ray from each fragment or pixel, respectively, doing all the compute for a single ray in the shader.

- Fragment Shader:  We've seen these before.  We're doing very similar things as we did in Lab 7 and 8 with frame buffers.  The difference is that instead of doing blur compute, we're writing a whole ray tracer in the fragment shader!

- Compute Shader:  New in OpenGL 4.3 (don't use mac os for this), are compute shaders.  These shaders aren't part of the OpenGL pipeline (vertex -> fragment -> framebuffer -> out).  Instead, it's a more general way to access threads on your graphics card then having to pass through the OpenGL pipeline (GPGPU).  Compute shaders are probably overkill for our scenes, but as we were interested in GPGPU, it's a good place to start in order to move from OpenGL to something like CUDA or OpenCL. 

