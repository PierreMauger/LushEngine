# <p align="center">🌿 Lush Engine 🌿</p>

## <p align="center">C++ Engine made with OpenGL</p>

## 📋 Table of Contents
<details>
<summary>Click to reveal</summary>

- [About](#-about)
- [Features](#-features)
- [Requirements](#-requirements)
- [Building](#-building)
- [Code](#-code)
- [Authors](#-authors)

</details>

## 🔍 About

This project is about making a cross-platform 3D engine in C++, mainly as an excuse to work on cool 3D stuff on my free time.\
It has an ECS and an implementation of ImGui for debuging.\
The rendering is made in OpenGL with the intent of staying as low level as possible. Implementation of Vulkan is planned for MacOS users.

## 📈 Features

Most features are still under developpement. Here are the main ones:

- Loading 3D models
- Mouse picking on 3D objects
- 3D skeletal animation
- Tesselation of terrain (LOD)
- Advanced lighting using shaders

## 💻 Requirement

Libraries are handled by CMake, the required librairies are fetched from their Git Release.

List of external libraries used:
- OpenGL
- GLFW
- GLEW
- ASSIMP (for models and animations import only)

## 🚧 Building

The project provide two scripts.

Use `build.sh` (Linux/MacOS) or `build.bat` (Windows) to start building the project.\
It will automatically:
- Fetch the dependancies
- Build the internal dynamic libraries (.so/.dll)
- Build the code of the project
- Create the executable (`lush` or `lush.exe` for Windows)

## 📉 Code

The main code is inside the Systems.\
To implement a new system, you just need to bind it to the SystemManager in the Engine.cpp (a system follows an interface ISystem).\
To implement a new component, add it inside ComponentTypes.hpp, and bind it to the ComponentManager. You will then need to add it manually inside the GUI and the Component list, follow implementation of other components. Sorry and good luck :)\

## 📣 Authors

Made by [Pierre MAUGER](https://github.com/PierreMauger)
