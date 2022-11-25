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
The rendering is made in OpenGL with the intent of staying as low level as possible. Implementation of Vulkan is planned in the future.

## 📈 Features

Most features are still in development. Here are the main ones:

- Loading 3D models
- 3D skeletal animation
- Mouse picking on 3D objects
- 3D camera, first and third person
- Tesselation of terrain (LOD)
- Advanced lighting using shaders
- Cubemaps
- Billboards (LOD and UI)
- Engine UI
- Lua scripting (in development)

## 💻 Requirements

Libraries are handled by CMake, the required librairies are fetched from their Git Release.\
Assimp is temporarily not fetched due to it's size. Install it on your machine or enable the script manually.

List of external libraries used:
- OpenGL
- GLFW
- GLEW
- ASSIMP (for import, will be replaced)

## 🚧 Building

The project provide two scripts for building.\
Use `build.sh` (Linux/MacOS) or `build.bat` (Windows) to start building the project.

It will automatically:
- Fetch the dependancies
- Build the internal dynamic libraries (.so/.dll)
- Build the code of the project
- Create the executable (`lush` or `lush.exe` for Windows)

## 📉 Code

To implement game logic, the code will be moved from custom systems to Lua scripts (still in development).\
You will be able to write scripts and bind them to any entity, to interact with their components.
To create a new component, add it in the ComponentTypes list.

## 📣 Authors

Made by [Pierre MAUGER](https://github.com/PierreMauger)
