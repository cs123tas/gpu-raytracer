# GPU Raytracing:

## Idea: 
Use the graphics card for doing our ray tracing computations in parallel.

### How:
We can render our scene to a textured fullscreen quad, doing all the compute in a shader.  In this way, we cast a ray from each fragment or pixel, respectively, doing all the compute for a single ray in the shader.  We tried this two ways:

- Compute Shader: Compute Shaders are new to OpenGL 4.3, and are useful for the space between GPGPU and Graphics.  The idea is that we submit a ray trace job per pixel, without using any of the rasterization pipeline.  Unfortunately, we weren't able to connect the render to the blank texture.  So instead of going this route, we pivoted to something we already know: fragment shaders.

- Fragment Shader:  We've seen these before.  We're doing very similar things as we did in Lab 7 and 8 with frame buffers.  The difference is that instead of doing blur compute, we're writing a whole ray tracer in the fragment shader!  The idea here is that rays are cast from each corner of the quad and the fragment shader interpolates between rays in the quad.

## Who did what:
### Dan (ray tracing in GLSL)
- I worked on rendering the fragment shader.  This meant consuming fragment coordinates from the vertex shader and doing ray tracing inside the shader.  There's considerable things to keep in mind how this changes from doing our Ray assignment in C++:
	
	- The shader is one ray trace job, and we do this per fragment coordinate on the film plane.
	- Recursion is non-trivial in a shader. So I converted recursion to an iterative algorithm, in which we ping the rays one by one.  Importantly, we render reflections and refractions of reflections, but not reflections and refractions of refractions.  I tried going down this route but it was challenging without a stack-like data structure for jobs.
	- Adding refractions and also refracting each wavelength differently depending on the IOR of materials.
	- Fresnel Equations for determining how much light is reflected/refracted
	- Ironically, I was going for realism, but while playing with coefficients, I found a nice toon looking render.  Rather than play with parameters further, I just stayed with it.  We also didn't use image textures so modelling things like metal photorealistically was challenging.  I pretty much experimented with coefficients until I found something that I liked.  I think messing with Fresnel gave this cool bright highlights around the edges.
	- To demonstrate the GPU capabilities, I passed in a timer to oscillate the spheres instead of rendering a static scene.
	- Amir also helped me with some general ideas and some debugging.

### Amir (UI + Physics)



## NOTES:
	
	- I had to turn down the fps becauase of my computer using the parameter `m_sleepTime`.  Feel free to change this.




