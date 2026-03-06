## CRSP Engine
This repository contains my first custom Vulkan C++ game engine. With this project, I aim to familiarize myself more with graphics programming, experiment with different concepts such as ECS, and just have some fun.
The engine contains a simple Vulkan rendering pipeline with dynamic batched rendering, and a simple ECS implementation for the game logic. A sample SnakeGame and Boids scene are implemented to show the engine usage.
![SnakeScene](https://github.com/user-attachments/assets/69a9a8b9-2ff1-4a21-a19b-02bd437a2ac5)

# Dependencies
- glfw3
- glm
- Freetype

Dependencies are managed through vcpkg.

Install vcpkg:

git clone https://github.com/microsoft/vcpkg/bootstrap-vcpkg.sh   # Linux/macOS
git clone https://github.com/microsoft/vcpkg/bootstrap-vcpkg.bat   # Windows

Then you can build with CMake, and run the generated CrspEngine.exe

# Scenes
To change between scenes, for now you can go to main.cpp and change the Scene class on line 13 to either 'SnakeScene' or 'BoidsScene', or any other class inherited from the base Scene class.

# Features
- Vulkan graphics API
- Batched rendering
  - Transform data is uploaded to a single buffer, and objects with shared materials and meshes are drawn with single draw calls
- Materials
- Texture and Model loading
  - Basic TinyObjLoader and stb_image implementation
- Font-atlas generation and text rendering
  - Freetype is used to generate a font-atlas from a .ttf file
  - Text render component generates a mesh with vertices mapped to the glyphs on the atlas
- Basic ECS implementation
- Basic Shadow rendering
- Snake
- Boids (WIP)

# To-do
- Separate pipeline logic from materials
- Improved texture packing for font-atlas
- ECS Archetypes
- Shadow Cascades
- Shadows actually following view-frustum
- Proper UI framework
- Abstract rendering layer to support multiple rendering API
- A lot...
