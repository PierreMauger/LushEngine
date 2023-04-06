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

This project is about making a cross-platform 3D engine in C++, mainly as an excuse to work on cool 3D stuff and design paterns on my free time.\
It uses an ECS architecture and an implementation of ImGui for debugging.\
The engine propose C# scripting for game logic implementation.\
The rendering is made in OpenGL with the intent of staying as low level as possible. Implementation of Vulkan is planned in the future.

## 📈 Features

Most features are still in development. Here are the main ones:

- Loading 3D models
- 3D skeletal animation
- Mouse picking on 3D objects (using depth calculation)
- 3D camera, first and third person
- Tesselation of terrain (LOD)
- Advanced lighting using shaders
- Cubemaps
- Billboards (for LOD and UI)
- Engine docked UI (Unity-like)
- Guizmos (Unity-like)
- C# scripting (in development)

## 💻 Requirements

Libraries are handled by CMake, the required librairies are fetched from their Git Release.\
Some packets are not fetched due to their size and build cost. They will be provided in the repository.

List of external libraries used:
- OpenGL
- GLFW
- GLEW
- GLM
- ASSIMP
- Mono

## 🚧 Building

The project provide two scripts for building.\
Use `build.sh` (Linux/MacOS) or `build.bat` (Windows) to start building the project.

It will automatically:
- Fetch the dependencies
- Build the internal dynamic libraries (.so/.dll)
- Build the code of the project
- Create the executable (`lush` or `lush.exe` for Windows)
- Create the executable (`lushEditor` or `lushEditor.exe` for Windows)

## 📉 Code

To implement game logic, you can create a C# script and bind it to any entity.\
The script will be compiled and loaded at runtime.\
The script will be able to interact with the entity's components as long as it follows the basic interface (onInit, onUpdate, etc).

## 📣 Authors

Made by [Pierre MAUGER](https://github.com/PierreMauger)
