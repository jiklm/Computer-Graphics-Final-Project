# OpenGL Interactive 3D Viewer

This project is an interactive **3D graphics viewer** built with **OpenGL**, **FreeGLUT**, **GLUI**, and **tinyfiledialogs**.  
It supports rendering a teapot, a color cube, and user-loaded models in `.tri` format with lighting, shading, and material controls.

---

## Features
- Render **Teapot**, **Color Cube**, or external **.tri models**
- Switch between **Orthographic** and **Perspective** projection
- Interactive **Translation / Rotation / Scaling**
- **Lighting Control**: 3 configurable light sources (point, directional, spotlight)
- **Material Control**: Built-in presets (Gold, Silver, Copper, Chrome, Pewter)
- **Keyboard Controls**:
  - `WASD` → Move object
  - `Q / E` → Adjust rotation speed
  - `F1 / F2 / F3` → Change teapot color
- **Mouse Dragging** → Adjust background color

---

## Requirements
- C++ compiler (C++11 or later)
- OpenGL
- FreeGLUT
- GLUI
- tinyfiledialogs

---

## Model Loading

This viewer supports loading 3D models in a simple **`.tri` format**.  
You can load a model by clicking the **"Choose file"** button in the GUI.
The program supports .tri file format. Use the "Choose file" button to load a model.
Example `.tri` format:
```
Color: 2
Vertices: 4
0 0.0 0.0 0.0  0 0 1
1 1.0 0.0 0.0  0 0 1
2 0.0 1.0 0.0  0 0 1
3 0.0 0.0 1.0  0 0 1
0 0 1 2  255 0 0  0 255 0
1 1 2 3  0 0 255  255 255 0
```
