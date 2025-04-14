# Legendary 3D Engine ✨

![C++](https://img.shields.io/badge/language-C%2B%2B-blue.svg) ![OpenGL](https://img.shields.io/badge/API-OpenGL-green.svg) ![GLUT](https://img.shields.io/badge/Library-GLUT-orange.svg) ![GLEW](https://img.shields.io/badge/Library-GLEW-yellow.svg) ![CImg](https://img.shields.io/badge/Library-CImg-lightgrey.svg)

Welcome to the Legendary 3D Engine! This project is a software-based 3D graphics engine built from the ground up using C++ and leveraging libraries like OpenGL/GLUT/GLEW for windowing and context creation, and CImg for texture loading[cite: 1, 120]. It demonstrates fundamental concepts of 3D graphics rendering, including mesh loading, texturing, camera control, and a custom rasterization pipeline.

---

## 🌟 Showcase

* **Screenshots:**.

  ![image](https://github.com/user-attachments/assets/380f20e2-6473-4959-b3d0-ccf10a44c567)

  ![image](https://github.com/user-attachments/assets/e24f6cba-02f1-4cbf-8be9-b6f721b26ea4)

---  
* **Video/GIF:**

    [![Watch on YouTube](https://img.youtube.com/vi/Q9wKBAebM5s/maxresdefault.jpg)](https://youtu.be/Q9wKBAebM5s)

---

## 🚀 Core Features


- **Custom 3D Model Format**: Loads vertex and texture coordinate data from `.txt` files derived from the Wavefront OBJ format. Supports UV coordinates for texturing.
  
- **Texture Mapping**: Applies textures loaded from `.bmp` image files using the CImg library. Texture coordinates are perspective-correct using the `w` component.

- **Software Rasterization Pipeline**: Implements the rendering process without relying heavily on OpenGL's fixed-function or shader pipelines (beyond basic window setup and pixel blitting):
  - **Model Transformation**: Objects can have initial placement and rotation.
  - **View Transformation**: Uses a camera model to transform vertices into view space (`Camera::pointAt`).
  - **Projection Transformation**: Applies perspective projection to simulate depth.
  - **Back-face Culling**: Ignores triangles facing away from the camera for efficiency.
  - **Clipping**:
    - **Near Plane Clipping**: Clips triangles against the near viewing plane to prevent rendering issues.
    - **Screen Space Clipping**: Clips triangles against the 4 edges of the screen view (-1 to +1 in normalized coordinates).
  - **Rasterization**: Fills triangles pixel by pixel (`rasterize` function), interpolating texture coordinates.
  - **Depth Buffering**: Ensures correct pixel depth ordering using a Z-buffer (`DeftBuffer`).

- **Lighting**: Implements basic directional lighting with ambient and diffuse components calculated per triangle.

- **First-Person Camera**: Allows navigation through the 3D scene with adjustable movement and turning speeds.

- **Performance Monitoring**: Displays the current Frames Per Second (FPS) in the window title, adapting `deltaTime` for smoother movement.

- **Upscaled Rendering**: Renders the scene to an internal buffer at a lower resolution (`screenWidth`, `screenHeight`) and then blits (copies) it to a larger window using nearest-neighbor scaling for a retro aesthetic.
---

## 🛠️ Dependencies & Environment

* **Language:** C++ (using features potentially up to C++11/17 based on includes and syntax)
* **Platform:** Windows (uses `<Windows.h>` for `GetAsyncKeyState` and console hiding)
* **Graphics API:** OpenGL (via GLEW and FreeGLUT)
* **Libraries:**
    * **GLEW:** Manages OpenGL extensions.
    * **FreeGLUT:** Handles window creation, input, and the main loop.
    * **CImg:** Used for loading BMP texture files.
* **Compiler:** Requires a C++ compiler compatible with the libraries (e.g., Visual Studio).

---

## ⚙️ Setup & Compilation (Visual Studio Example)

1.  **Clone:** Get the source code:
    ```bash
    git clone https://github.com/LuDeVing/3D-Engine.git
    ```
2.  **Dependencies:**
    * Download or install FreeGLUT, GLEW, and CImg.
    * Configure your Visual Studio project:
        * **Project Properties -> VC++ Directories:**
            * Add paths to the `include` directories of FreeGLUT, GLEW, and CImg to `Include Directories`.
            * Add paths to the `lib` directories of FreeGLUT and GLEW to `Library Directories`.
        * **Project Properties -> Linker -> Input:**
            * Add `freeglut.lib`, `glew32.lib` (or similar), and potentially `opengl32.lib` to `Additional Dependencies`.
    * Ensure the necessary DLLs (like `freeglut.dll`, `glew32.dll`) are accessible at runtime (e.g., in the same directory as the executable or in the system path).
    * Place the CImg header (`CImg.h`) where your project can include it.
3.  **Build:** Compile the solution in Visual Studio (e.g., using `Build -> Build Solution`).
4.  **Run:**
    * Execute the generated `.exe` file from the build output directory (e.g., `Debug` or `Release`).
    * Make sure the `meshes` and `textures` folders containing the map and texture filesare placed correctly relative to the executable, or adjust the file paths in `main.cpp`.
    * A console window will briefly appear asking you to choose a map (1, 2, or 3) before hiding itself. Enter your choice.

---

## 🎮 Controls

Navigate the rendered 3D environment using the following keyboard controls:

* **Movement:**
    * `W`: Move Forward
    * `S`: Move Backward
    * `Space`: Move Up
    * `C`: Move Down
* **Rotation:**
    * `A`: Turn Left
    * `D`: Turn Right
* **Speed/Sensitivity Adjustment:**
    * `Numpad +` / `Numpad -`: Fine-tune movement speed.
    * `F1` / `F2`: Significantly increase/decrease movement speed.
    * `F3` / `F4`: Increase/decrease turning sensitivity.

---

## 📁 Code Structure Overview

* `main.cpp`: Entry point, GLUT/GLEW initialization, main loop (`makeFrame`), input handling (`getInputs`), texture loading, mesh selection, calling rasterization (`render`).
* `translateAndNormalize.h`: Contains the `object` class responsible for the core rendering pipeline logic: view/projection transformations (`normalise`), clipping (`triangleClipping`), and sorting (commented out).
* `camera.h`: Defines the `Camera` class, primarily the `pointAt` function to calculate the view matrix.
* `lighting.h`: Simple lighting calculation (`lightValue`) based on surface normal, light direction, and ambient color.
* `structuresandhelperfunctions.h`: Defines fundamental data structures (`point`, `triangle`, `mat`, `mesh`, `Pixel`, `DisplayImage`), vector math operations, plane-line intersection (`plane_lineIntersection`), triangle clipping logic (`triangleClipping`), and the mesh loading function (`LoadFromObjectFile`).
* `cimg.h`: External CImg library header for image operations.
* `meshes/`: Contains the 3D model data files.
* `textures/`: Contains the BMP texture files corresponding to the meshes.

---

Enjoy exploring the engine!
