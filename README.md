# No Graphics Library
Idk really if it will be a library or a standalone.\
It is forked from OMGL, I hate OpenGL as of now, so I decided to split paths with it and replace it with NO GRAPHICS LIBRARY!

# Current state
As of 2024-09-25, projecting vertices and rendering triangles takes 11-20ms, no face culling yet(got work to do).\
![Rendering mid-poly model's triangles at 17ms](screenshots/screenshot-25-09-24.png)

# Roadmap(TODO)
- [x] Basics.
  - [x] Pushing the project.(I want something to be ticked lol)
  - [x] Set up a performant Window.
  - [x] Setting up the basics.
  - [x] Basic loop.
- [x] Synchronization primitives
  - [x] Thread objects.
  - [x] Atomic objects(basics, not everything).
  - [x] Fast mutex objects.
  - [x] Event based objects(Bells).
- [ ] Math utilities. (Ongoing)
  - [x] Understand SIMD enough to use it.
  - [x] Play around with SIMD and prototype stuff for the below step.
  - [x] Vector math, using SIMD and bulk computation of vectors. The idea is a VOV(Vector Of Vectors) object.
  - [x] Matrix math for perspective projection.
  - [x] Various vector arithmetic using SIMD: normalization, dot, cross, etc.
  - [x] Refector hot functions like multiplication with matrices for multi-thread support.
  - [ ] Polish up.
- [ ] Multi-threading.
  - [x] Minions, split work between each other, like multiplying VOVs.
  - [ ] Polish up.
- [ ] Loading models(glTF format(.glb only for now))
  - [x] Refer to [glTF](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html) for docs(Ongoing).
  - [x] Make a JSON parser, integral part of glTF.
  - [x] Look for any leaks and stuff in the JSON parser.
  - [x] Bare bones load models into a Scene object using utilities I made.
  - [x] Load camera info into the Scene too or at least implement logic for cameras.
  - [x] Make a PNG parser, I ain't dealing with it myself so with libpng.
  - [ ] Texture support, TEXCOORDS_n and shii.
  - [ ] Optional - Add support for glTF text version.
- [ ] Rendering.
  - [x] Projecting model's vertices into screen space(Very optimized because screen space scaling is in the projection matrix!).
  - [x] Rendering vertices themselves.
  - [x] Rendering triangles, basic incremental half-spaced method.
  - [x] Advanced block based triangle rendering.
- [ ] Advanced shading to prepare for next step.
  - [ ] Specular highlighting.
  - [ ] Metallic shaders.
  - [ ] Sheen.
  - [ ] Sub-surface scattering, a cool one.
  - [ ] Understand and tackle some PBR.
- [ ] Writing shaders for more complex maps. WITH PBR!
  - [ ] Good ol' bump maps.
  - [ ] Normal maps.
  - [ ] Specular maps.
  - [ ] The rest of the stuff.
- [ ] Post processing.
  - [ ] Rendering to a texture.
  - [ ] Applying simple anti-aliasing.
  - [ ] Bloom.
  - [ ] Film, or just general grain.
  - [ ] Palettizing.
- [ ] Rigging.
- [ ] Animation?
- [ ] Extras.
  - [ ] Implement for Linux, not that complex.
  - [ ] Font rendering. For now monospaced.
  - [ ] Using the new font renderer to render useful info.

# Basic making
Your usual:
```sh
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
nogl
```

# Making on Linux for Windows
use `cmake` with the `toolchains/unix-win32.cmake` toolchain. In a nutshell:
```sh
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=toolchains/unix-win32.cmake -DCMAKE_BUILD_TYPE=Release ..
make
wine nogl.exe
```
