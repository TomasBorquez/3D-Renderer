# About
3D render CPU only, to build it uses my [own build system](https://github.com/TomasBorquez/mate.h) which you can run with any compiler as `gcc mate.c -o mate && ./mate`,
vendor packages for SDL are only on windows so unless you install them yourself this is only runnable on windows.

## Goal
- [x] Display simple shapes on the screen. Cube and Piramid
- [x] Parse simple obj files
- [x] Parse simple obj with textures and colors
- [ ] Fix matrix math
- [ ] Just make a huge array of pixels and send them after each frame
- [ ] Create scenes
- [ ] Add movement controls
- [ ] Add simple light like Goren
- [ ] Copy scene from Goren
- [ ] Some multi-threading?
- [ ] Create my own demo

## Resources
These are the docs/books/inspirations used to make this renderer:

**Books**:
- ModernC Book 
- GNU LibC Book
- LearnOpenGL Book
- Windows Programming
- Mathematics of 3D Graphics Book
- An Introduction to GCC (University of Pennsylvania) Book

**Manuals**:
- Man Pages
- GCC Manual
- Clang Manual
- SDL3 Offline Wiki

**Github**:
- ssloy/tinykaboom
- austinmpask/Goren
- ssloy/tinyrenderer
- skywind3000/mini3d
- ssloy/tinyraycaster
- claudemuller/3d-renderer.c
