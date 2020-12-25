# Oxy 2

![Dragon](images/dragon.png)
Transmissive Stanford Dragon, 8192 samples, 871k triangles, 16 minute render on 24 threads

## In progress:
* XSDL
    * Basically want this to be a functional scripting language
    * Also wanna write a JIT x86 compiler so I can define SDF's in a file
* Rendering
    * Bidirectional path tracing

## Features:

* AVX2 / BVH accelerator for meshes
* SDL (Scene description language) for defining scenes
* Shitty rectilinear camera model with a tent filter
* Crappy naive pathtracing with russian roulette pruning
* Materials
  * General purpose BSDF'ish thing
* Textures
  * Images
  * Solid color
* SDF (Signed distance field) rendering
  * No way of defining these through the SDL right now

## To-do:

* Distributed rendering over the network
* Better surface normal calculation
* Smooth hitnormals for meshes
* Subsurface scattering
* Environment spheres
  * HDRi textures
  * Atmospheric scattering models
* Mesh instancing
* Lens model for the camera
* Generative textures
* Extended SDL

## More renders

![More caustics](images/caustic3.png)
Version of above image with some extra lights, rendered in 4K

![Caustic](images/caustic.png)
Different colored lights mixing through a sphere

![Glass](images/glass2.png)
Glass and a glossy red sphere, light source up top and some ambient lighting
