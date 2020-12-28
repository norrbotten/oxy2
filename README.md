# Oxy 2

![Dragon](images/pssmlt_4k.png)
Standford Dragon rendered with the PSSMLT integrator

## In progress:
* XSDL
   * Basically want this to be a functional scripting language
   * Also wanna write a JIT x86 compiler so I can define SDF's in a file with no real overhead
* Rendering
   * Metropolis integrator

## Features:

* AVX2 / BVH accelerator for meshes
* SDL (Scene description language) for defining scenes
* PSSMLT (Primary sample space Metropolis light transport)
  * No idea if i'm doing it properly, but caustics converge in way fewer samples
* Naive/implicit integrator
  * Converges direct light very fast, but caustics look like ass
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

![Monke](images/ring_caustic.png)
Blender Suzanne in a ring

![Caustic](images/caustic.png)
Different colored lights mixing through a sphere

![Glass](images/glass2.png)
Glass and a glossy red sphere, light source up top and some ambient lighting
