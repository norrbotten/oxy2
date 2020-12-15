# Oxy 2

![Caustic](/images/caustic.png)
Different colored lights mixing through a sphere

## Features:

* SDL (Scene description language) for defining scenes
* Shitty rectilinear camera model with a tent filter
* Crappy naive pathtracing with russian roulette pruning
* Materials
  * General purpose BSDF'ish thing which isnt even done yet
  * Emissive with output energy
* Textures
  * Images
  * Solid color
* SDF (Signed distance field) rendering
  * No way of defining these through the SDL right now

## To-do:

* Finish the BSDF material
* Environment spheres
  * HDRi textures
  * Atmospheric scattering models
* Mesh loading and instancing
* Primitive level BVH and AVX/SSE for triangle intersection
  * More or less already finished in Oxy 1, just need to integrate in this project
* Lens model for the camera
* Generative textures
* Extended SDL

## More renders

<img src=/images/glass2.png width=512>
Glass and a glossy red sphere, light source up top and some ambient lighting

<img src=/images/spheres_20000.png width=512>
Repeated spheres defined with SDF, light source in the center


<img src=/images/cornell1.png width=512>
Torus with a transform


<img src=/images/glass.png width=512>
Slightly green glass sphere
