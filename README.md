# No Graphics Library
Idk really if it will be a library or a standalone.\
It is forked from OMGL, I hate OpenGL as of now, so I decided to split paths with it and replace it with NO GRAPHICS LIBRARY!

# Roadmap(TODO)
- [ ] Basics.
  - [x] Pushing the project.(I want something to be ticked lol)
  - [x] Set up a performant Window.
  - [x] Setting up the basics.
  - [x] Basic loop.
  - [ ] TODO
- [ ] Advanced multi-source lighting.
- [ ] Camera for viewing.
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
cmake ..
make
```

# Making on Linux for Windows
use `cmake` with the `toolchains/unix-win32.cmake` toolchain. In a nutshell:
```sh
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=toolchains/unix-win32.cmake ..
make
```
