# 🎮 OpenGL Component-Based Renderer

This is a custom 3D graphics renderer built in C++ using **OpenGL 3.3 (Core Profile)**, GLFW, and GLEW. 

The architecture is heavily inspired by **Unity Engine's GameObject-Component system**, enabling modular scene composition through entity components.

---

## 📂 Project Structure

- **`OpenGL_Graphics/`**: Contains the source code of the rendering engine.
  - **`Core/`**: Core engine initialization (`Engine`), Game Framework entry point (`GameFramework`), and Shader management.
  - **`Entities/`**: Entity definition (`GameObject`) representing any object in the scene.
  - **`Components/`**: Components (`TransformComponent`, `MeshComponent`, `KeyboardController`) attached to GameObjects to extend functionality.
  - **`Managers/`**: Global subsystems managing active scenes (`SceneManager`), inputs (`InputManager`), camera (`CameraManager`), and resource caches (`ResourceManager`).
  - **`Shaders/`**: Vertex and fragment shaders (`cube.vs`, `cube.fs`).
- **`docs/`**: Project documentation.
  - [OpenGL_Renderer_Roadmap.md](file:///c:/Users/ckswl/Desktop/GITHUB/OpenGL/docs/OpenGL_Renderer_Roadmap.md): Detail project design specifications and future development milestones (Lighting, PBR, IBL, and Scene Editor with ImGui).
- **`.agents/`**: AI assistant custom workspace skills.
  - Contains specialized reference rules for `shader-programming`, `lighting-design`, `unity-development`, and `game-development`.

---

## 🎯 Documentation & Roadmap

For the detailed plan regarding lighting, PBR, and asset placement, please refer to:
👉 **[OpenGL Renderer Roadmap & Design Specification](file:///c:/Users/ckswl/Desktop/GITHUB/OpenGL/docs/OpenGL_Renderer_Roadmap.md)**
