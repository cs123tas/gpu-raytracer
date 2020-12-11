# GPU Raytracing + Rigid Body Dynamics Simulation:

## Idea: 
Use the graphics card for doing our ray tracing computations in parallel.

### How:
We can render our scene to a textured fullscreen quad, doing all the compute in a shader.  In this way, we cast a ray from each fragment or pixel, respectively, doing all the compute for a single ray in the shader.  We tried this two ways:

- Compute Shader: Compute Shaders are new to OpenGL 4.3, and are useful for the space between GPGPU and Graphics.  The idea is that we submit a ray trace job per pixel, without using any of the rasterization pipeline.  Unfortunately, we weren't able to connect the render to the blank texture.  So instead of going this route, we pivoted to something we already know: fragment shaders.  I only kept some of this stuff in the repo, but feel free to ignore it.

- Fragment Shader:  We've seen these before.  We're doing very similar things as we did in Lab 7 and 8 with frame buffers.  The difference is that instead of doing blur compute, we're writing a whole ray tracer in the fragment shader!  The idea here is that rays are cast from each corner of the quad and the fragment shader interpolates between rays in the quad.  We render this color to a color attachment on an FBO, because although we didn't get to it by the deadline, we could then use the texture in the FBO to do post processing effects like motion blur.  

## Who did what:
### Dan (ray tracing on GPU)
- I worked on rendering the fragment shader.  This meant consuming fragment coordinates from the vertex shader and doing ray tracing inside the shader. There's considerable things to keep in mind how this changes from doing our Ray assignment in C++:
	
	- The shader is one ray trace job, and we do this per fragment coordinate on the film plane.
	- Recursion is non-trivial in a shader. So I converted recursion to an iterative algorithm, in which we ping the rays one by one. Importantly, we render reflections and refractions of reflections, but not reflections and refractions of refractions. I tried going down this route but it was challenging without a stack-like data structure (while stack not empty, pop ray, refract reflect, etc.)
	- Adding refractions and also refracting each wavelength differently depending on the IOR of materials.  I didn't get the chromatic abberations I wanted but I can clearly see different "bands" of refractions on the center sphere.
	- Fresnel Equations for determining how much light is reflected/refracted
	- Ironically, I was going for realism, but while playing with coefficients, I found a nice toon looking render.  Rather than play with parameters further, I just stayed with it.  We also didn't use image textures so modelling things like metal photorealistically was challenging.  I pretty much experimented with coefficients until I found something that I aesthically liked.  I think messing with Fresnel gave this cool bright highlights around the edges, which I appreciated.
	- To demonstrate the GPU capabilities, I passed in a timer to oscillate the spheres instead of rendering a static scene.  This was boiler plate before Amir got the physics sim up and running.
	- Amir also helped me with some general ideas and some shader debugging.

### Amir (UI + Rigid Body Physics)
- I worked on implementing a method to simulate rigid body dynamics and added a UI to our project and implemented Newton equations to compute force, velocity and position vectors given constant forces of gravity and mass of the objects. I also implemented a basic collision detection and avoidance with the walls. The initial goal was to see real-time physical simulation and ray tracing on the GPU but it did not happen due to the difficunty in implementing Raytracing using Compute Shaders that Dan explained above.
	- For the physics, the simulation starts with some spheres in a fixed position and gravity pulls them down at different velocities due to their differnet masses. Then once the spheres hit the floor they start bouncing off to random directions (mainly guided by the Normal of the surface they hit) and this process keeps continueing indefinitly.
	- I did the physics simulation outside of the raytracing shader and pass the updated position vectors for the objects to the shader via Uniforms.
	- The biggest challenge for me came from the fact that our renderer does orthographic projection of the objects. This made physics debugging very hard as I could not be confident about the result of my computation. We tried fixing this but we could not figure out why perspective rendering does not work.
	- Another difficulty was not having a real-time raytracer which made debugging even harder but I tried to partially get around this a bit by using an older, and less computationally demanding, version of the renderer that Dan wrote.
	- One thing that took me a very long time to fix was the fact that most of my computations in our View::paintWithFragmentShaders() function would return incorrect results. I'm still not sure why that is the case. It seems that although I had a vector that contained the spheres' data, somehow the shader would only read that data from a particular block of the memory. The same exact computation would do the right thing outside of View::paintWithFragmentShaders() but that wasn't useful because I had to run it in View::paintWithFragmentShaders() to update the values for each frame. What I did was to decouple everything as much as possible to fix this (e.g. instead of having a vector of spheres I declared 3 differnet struct instances for each sphere)
- Finally, note that I pushed the latest version of my code to the physics branch of the repo.


## NOTES:
	In View.cpp
	- I had to turn down the fps because of my computer using the parameter `m_sleepTime`, which stops the program from updating too frequently.  Feel free to change this.  
	- Also note that I switched to Windows 10 from Mac to work on this as not all the newer OpenGL features are supported.  I think things should generally work.  Amir was able to run the raytracer on Linux.




