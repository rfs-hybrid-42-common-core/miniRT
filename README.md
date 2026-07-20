*This project has been created as part of the 42 curriculum by maaugust, fraalexa.*

<div align="center">
  <img src="https://raw.githubusercontent.com/rfs-hybrid/42-Common-Core/main/assets/covers/cover-minirt-bonus.png" alt="MiniRT Cover" width="100%" />
</div>

<div align="center">
  <h1>🎇 miniRT: My first RayTracer with MiniLibX</h1>
  <img src="https://img.shields.io/badge/Language-C-blue" />
  <img src="https://img.shields.io/badge/Grade-125%2F100-success" />
  <img src="https://img.shields.io/badge/Norminette-Passing-success" />
</div>

---

## 💡 Description
**miniRT** is an introduction to the beautiful world of Raytracing. The goal of this program is to generate images using the Raytracing protocol, representing a scene defined by simple geometric objects and its own lighting system. 

Unlike rasterization, which is used by most graphics engines for its efficiency, ray tracing simulates the physical behavior of light to produce a much higher degree of visual realism. This project was built entirely from scratch in C using the MiniLibX library, with no external mathematical dependencies. It evolved from a humble mandatory ray tracer into a highly optimized, multithreaded rendering engine featuring Bounding Volume Hierarchies (BVH), Constructive Solid Geometry (CSG), physically based optical refractions, and complex procedural textures.

---

## 📑 Features

### 🔹 Mandatory Features
* **Primitive Geometry:** Complete support for calculating mathematical intersections and handling the insides of planes, spheres, and cylinders.
* **Transformations:** Full translation and rotation transformations applied natively to objects, cameras, and lights. Resizing is supported for unique properties like sphere diameters and cylinder dimensions.
* **Lighting Engine:** Implementation of spot brightness, hard shadows, and ambient/diffuse lighting based on the Blinn-Phong reflection model.
* **Window Management:** Fluid window management using MiniLibX, guaranteeing clean exits via the ESC key or the window's red cross.

### 🚀 Bonus Features
* **Advanced Optics:** Specular reflections to achieve a full Phong reflection model. Implementation of Snell's Law and Christophe Schlick's approximation for realistic glass and water refraction.
* **Extended Geometry:** Mathematical support for second-degree objects like cones, as well as complex 4th-degree polynomials to render Toruses.
* **Constructive Solid Geometry (CSG):** Boolean math operations (Union, Intersection, Difference) to combine primitives into highly complex shapes.
* **External Mesh Parsing (`.obj`):** A custom, high-speed, two-pass OBJ loader capable of triangulating N-Gons and parsing massive 3D models.
* **Procedural Patterns & Disruption:** 3D volumetric checkerboards, stripes, gradients, and concentric rings. Includes advanced Perlin and Marble fractal noise generation.
* **Advanced Texture Mapping:** 2D UV mapping wrapping algorithms (Spherical, Planar, Cylindrical, etc.) for `.xpm` image textures.
* **Bump Mapping (TBN):** Support for tangent-space normal maps to fake geometric depth on flat surfaces.
* **Environmental Backgrounds:** Support for emissive 6-sided cubical Skyboxes and spherical HDRI Domes.
* **Soft Shadows:** Area lights utilizing Stratified Jittered Sampling on a sparse-grid optimization to render physically accurate penumbrae.
* **Motion Blur & Depth of Field:** Temporal ray interpolation for motion blur, and aperture focal-blur (Bokeh) integration.
* **Shearing Transformations:** 6-parameter skew matrices allowing for the creation of leaning or stretched geometry.

---

## 🧠 Algorithm & Data Structure

### 1. High-Performance Math & Physics Backend
* **Custom Linear Algebra:** Written entirely from scratch without external libraries. Features 4x4 matrix inversion, transposition, multiplication, and Gram-Schmidt orthogonalization.
* **Polynomial Solvers:** Implements a Quadratic solver for standard primitives, a Trigonometric Resolvent Cubic solver, and Ferrari’s method (Quartic) to accurately render Toruses.
* **Quaternion SLERP:** Uses Spherical Linear Interpolation (via Quaternions) instead of standard matrix LERP to interpolate object rotations during motion blur, completely eliminating volume-warping and Gimbal lock.

### 2. Spatial Partitioning: Bounding Volume Hierarchy (BVH)
To handle millions of triangles from parsed OBJ files without locking up the CPU, the engine uses a **BVH Tree** built on a Surface Area Heuristic (SAH).
* **Flat Array Optimization:** The tree is stored in a contiguous, statically-allocated array rather than a linked list to eliminate CPU cache misses.
* **Iterative Traversal:** Tree traversal avoids recursion entirely. It uses a predefined stack array `nodes_to_check[256]` to test rays against Axis-Aligned Bounding Boxes (AABBs), ensuring the engine never suffers a stack overflow.
* **Jim Arvo’s AABB Algorithm:** Bounding boxes are transformed efficiently without rotating all 8 corners, utilizing Arvo's tensor matrix multiplication.

### 3. Execution & Multithreading (POSIX)
* **Tile-Based Rendering Pool:** The screen resolution is mathematically sliced into 100x100 `t_tile` chunks.
* **Mutex Work-Stealing:** `NUM_THREADS` (typically 8) are spawned. Threads constantly lock a central mutex, steal the next available tile index, and render it independently. This guarantees 100% CPU saturation and perfect load balancing across all cores.
* **Zero-Malloc Intersections:** The core `intersect_object` loop tracks ray hits using a statically sized stack array (`MAX_INTERSECTIONS 128`), meaning the engine never calls `malloc` while rendering a frame.

### 4. Interactive "BIOS" Setup Utility
If launched without arguments, the engine boots into a custom UI built directly on the MiniLibX buffer. It scans the `scenes/working/` directory, sorts the `.rt` files alphabetically, and allows the user to navigate the file system, read the built-in parser documentation, and launch scenes interactively.

---

## 📖 Parser Strategy & Reference

Parsing operates on a massive 64KB block-reading buffer (`fast_read_line`) to eliminate the fragmentation overhead of `get_next_line`.

### The Core Rule: "Mandatory First, Tags Last"
The engine utilizes a strict parsing structure:
1. Every line must begin with the exact mandatory parameters in the exact mandatory order.
2. Any text remaining on the line after the mandatory parameters is treated as optional Bonus Tags (`key:value`).
3. If an optional tag is missing, the engine automatically applies the initial default settings.
4. If an optional tag is present but malformed, the engine must abort with an error.

### 1. Environment Elements
| Element | ID | Mandatory Base Format | Optional Bonus Tags |
| :--- | :--- | :--- | :--- |
| **Resolution** | `R` | `R [width] [height]` | *None* |
| **Ambient** | `A` | `A [ratio] [R,G,B]` | *None* |
| **Camera** | `C` | `C [pos] [forward_dir] [fov]` | `ap:[aperture]` `focal:[length]` |
| **Light** | `L` | `L [pos] [ratio] [R,G,B]` | `type:[spot/area]` `dir:[x,y,z]` `in:[deg]` `out:[deg]` `uvec:[x,y,z]` `ustep:[int]` `vvec:[x,y,z]` `vstep:[int]` |

### 2. Geometry Elements
| Shape | ID | Mandatory Base Format |
| :--- | :--- | :--- |
| **Sphere** | `sp` | `sp [pos] [diameter] [R,G,B]` |
| **Plane** | `pl` | `pl [pos] [normal_dir] [R,G,B]` |
| **Cube** | `cb` | `cb [pos] [normal_dir] [scale] [R,G,B]` |
| **Cylinder** | `cy` | `cy [pos] [normal_dir] [diameter] [height] [R,G,B]` |
| **Cone** | `cn` | `cn [pos] [normal_dir] [diameter] [height] [R,G,B]` |
| **Torus** | `to` | `to [pos] [normal_dir] [major_d] [minor_d] [R,G,B]` |
| **Triangle** | `tr` | `tr [p1] [p2] [p3] [uv1] [uv2] [uv3] [R,G,B]` |
| **Smooth Tri.** | `smt` | `smt [p1] [p2] [p3] [n1] [n2] [n3] [uv1] [uv2] [uv3] [R,G,B]` |
| **OBJ Model** | `obj` | `obj [pos] [normal_dir] [scale] [R,G,B] [filepath]` |

### 3. Structural Elements (Hierarchy & CSG)
Because flat text files lack brackets, hierarchy is achieved via string IDs.

| Element | ID | Mandatory Base Format | Usage Notes |
| :--- | :--- | :--- | :--- |
| **Group** | `grp` | `grp [pos] [normal_dir] [scale] id:[name]` | Apply `parent:[name]` to child objects. |
| **CSG** | `csg` | `csg [op] [left_id] [right_id] [pos] [norm] [scale]` | `op` can be: `union`, `inter`, `diff`. |

> **Deep Nesting & CSG Chaining:** Because `grp` and `csg` elements can also accept the `id:[name]` and `parent:[name]` tags, they can be chained infinitely. To nest groups, assign a group an ID and make it the parent of another. To chain CSG operations, feed a resulting CSG ID as the left or right ID of a completely new CSG line!

### 4. The Universal Bonus Tags Explained
These optional tags can be appended to the end of any geometry element.

**Material Overrides**
*   `mat:[amb],[diff],[spec],[shininess]`: Overrides the default Phong material properties.
*   `ref:[ratio]`: Makes the object a mirror (Range: 0.0 to 1.0).
*   `glass:[refractive_index],[transparency]`: Turns the object into refractive glass (e.g., `glass:1.5,1.0`).

**Procedural Patterns**
*   `chk:[R,G,B],[R,G,B]`: Applies standard 3D spatial checkers.
*   `str:[R,G,B],[R,G,B]`: Applies 3D spatial stripes.
*   `gra:[R,G,B],[R,G,B]`: Applies a 3D linear gradient between two colors.
*   `rng:[R,G,B],[R,G,B]`: Applies 3D concentric rings.
*   `marble:[R,G,B],[R,G,B],[octaves],[vein_scale],[pattern_scale]`: Generates 3D marble noise.
*   `perlin:[R,G,B],[R,G,B],[octaves],[pattern_scale]`: Generates 3D perlin noise.

**Texture Mapping (UV)**
*   `uv_chk:[width],[height],[R,G,B],[R,G,B]`: Applies 2D UV mapped checkers.
*   `img:[filepath]`: Applies an XPM image texture.
*   `bump:[filepath],[intensity],[epsilon]`: Applies an XPM normal/bump map.

**Modifiers (Transform, Motion & Hierarchy)**
*   `rot:[x],[y],[z]`: Applies local Euler angle rotations (Pitch, Yaw, Roll in degrees). Essential for rotating UV textures or bypassing the 2-degrees-of-freedom limit of standard normal vectors.
*   `shr:[xy],[xz],[yx],[yz],[zx],[zy]`: Applies a 6-parameter shearing (skew) matrix, allowing complex leaning/stretched shapes without altering intersection math.
*   `mov:[x,y,z]`: Sets the `transform_end` position (destination coordinate) for temporal motion blur interpolation.
*   `caps:[0/1]`: Toggles the end-caps on a Cylinder or Cone. `1` is closed (default), `0` is an open/hollow tube. *(Warning: CSG Boolean operations require watertight manifolds. Applying `caps:0` inside a CSG node may produce invalid intersection math.)*
*   `id:[name]`: Assigns a unique string ID to the object.
*   `parent:[name]`: Assigns the object as a child to a Group or CSG.

**Environment Backgrounds**
*   `dome:[filepath]`: Applied to a Sphere. Uses spherical mapping and automatically sets ambient to 1.0 (emissive) and diffuse/specular to 0.0 to prevent 3D shadows on the sky. Size is dictated by the Sphere's mandatory diameter.
*   `skybox:[right],[left],[up],[down],[front],[back]`: Applied to a Cube. Applies cubical skybox mapping requiring 6 distinct `.xpm` file paths. Automatically sets ambient to 1.0 (emissive) and diffuse/specular to 0.0. Size is dictated by the Cube's mandatory scale.

---

## 📸 Visual Showcase

### 1. The Setup (Interactive BIOS Utility)
To make exploring the engine effortless, `miniRT` features a fully custom, interactive UI that boots up if no `.rt` file is provided via the command line. This allows users to easily test error handling, review documentation, and load complex scenes without ever touching the terminal.

<table align="center">
  <tr>
    <th align="center" colspan="2">Main Menu</th>
  </tr>
  <tr>
    <td align="center" colspan="2"><img src="https://github.com/user-attachments/assets/93046bcc-c6ee-4aac-b89b-b2ab9b645a3b" alt="BIOS Main Menu" width="600"/></td>
  </tr>
  <tr>
    <th align="center">Working Scenes Directory</th>
    <th align="center">Error Handling & Broken Scene Tests</th>
  </tr>
  <tr>
    <td align="center"><img src="https://github.com/user-attachments/assets/e022b7d4-a68f-4e6f-969c-f018f7c8bdc7" alt="Working Scenes Directory" width="600"/></td>
    <td align="center"><img src="https://github.com/user-attachments/assets/275b8c46-a79b-4320-9a31-52c8d2bdcd94" alt="Error Handling & Broken Scene Tests" width="600"/></td>
  </tr>
  <tr>
    <th align="center">Built-in Parser Documentation</th>
    <th align="center">Built-in Controls Documentation</th>
  </tr>
  <tr>
    <td align="center"><img src="https://github.com/user-attachments/assets/b378f3b6-5b9d-4928-9d9d-6f3cb4694777" alt="Built-in Parser Documentation" width="600"/></td>
    <td align="center"><img src="https://github.com/user-attachments/assets/9107f2a1-c20a-43b6-b9b1-d1a0744ad563" alt="Built-in Controls Documentation" width="600"/></td>
  </tr>
</table>

---

### 2. Feature Comparisons & Post-Processing FX

#### 🔹 Illumination & Motion Dynamics
*Demonstrating single point-light sharp occlusion vs. stratified jittered grid area-light penumbrae, alongside temporal matrix interpolation for moving geometry.*

<table align="center">
  <tr>
    <th align="center">Hard Shadows (Point Light)</th>
    <th align="center">Soft Shadows (Area Light)</th>
    <th align="center">Temporal Motion Blur</th>
  </tr>
  <tr>
    <td align="center"><img src="https://github.com/user-attachments/assets/ac64fc52-f77a-4be3-8f64-aff15227e8ba" alt="Hard Shadows" width="400"/></td>
    <td align="center"><img src="https://github.com/user-attachments/assets/b23adb4b-a03c-43ee-9559-0691a3f4c021" alt="Soft Shadows" width="400"/></td>
    <td align="center"><img src="https://github.com/user-attachments/assets/872929fd-5ce4-4312-a989-614d5966584a" alt="Motion Blur" width="400"/></td>
  </tr>
</table>

#### 🔹 Camera Optics & Rendering Algorithms

<table align="center">
  <tr>
    <th align="center">Pinhole Camera (No Focal Blur)</th>
    <th align="center">Thin-Lens Aperture (Focal Blur Enabled)</th>
  </tr>
  <tr>
    <td align="center"><img src="https://github.com/user-attachments/assets/aabc43ba-5ff1-47cd-b7f2-d08c3b7bee8b" alt="No Focal Blur" width="600"/></td>
    <td align="center"><img src="https://github.com/user-attachments/assets/90854da8-b1a9-4ece-aed3-3b5ada65f278" alt="Focal Blur Enabled" width="600"/></td>
  </tr>
  <tr>
    <th align="center">Standard Single-Ray (Aliased)</th>
    <th align="center">16x Supersampled Anti-Aliasing</th>
  </tr>
  <tr>
    <td align="center"><img src="https://github.com/user-attachments/assets/33d4f63d-5c24-410e-b95a-01da27e49a40" alt="Standard Single-Ray Sampling" width="600"/></td>
    <td align="center"><img src="https://github.com/user-attachments/assets/11d9ae84-9fe0-4ca5-ba93-00333cba49d1" alt="16x Supersampled Anti-Aliasing" width="600"/></td>
  </tr>
</table>

---

### 3. Engine Stress-Test: The Solar System
*A complex multi-body astronomical scene demonstrating high-resolution spherical UV mapping, orbit geometry, custom skyboxes, and tangent-space Normal/Bump mapping.*

<table align="center">
  <tr>
    <th align="center">Panoramic System View</th>
  </tr>
  <tr>
    <td align="center"><img src="https://github.com/user-attachments/assets/b7cba807-0f19-48b5-b6b1-f0bd5dadf728" alt="Solar System Panoramic View"/></td>
  </tr>
  <tr>
    <th align="center">Zoomed System View</th>
  </tr>
  <tr>
    <td align="center"><img src="https://github.com/user-attachments/assets/68822255-29ea-4da3-8a60-dd4ef98e8abc" alt="Solar System Zoomed View"/></td>
  </tr>
  <tr>
    <th align="center">Tilted Orbital Plane</th>
  </tr>
  <tr>
    <td align="center"><img src="https://github.com/user-attachments/assets/2d0cdabc-575f-42e8-b30d-90f0df8a81c5" alt="Solar System Tilted Orbit View"/></td>
  </tr>
  </tr>
    <th align="center">Tangent-Space Bump Mapping</th>
  </tr>
  <tr>
    <td align="center"><img src="https://github.com/user-attachments/assets/4a00f743-2064-4a6b-8825-a036bf1e8772" alt="Earth and Moon Bump Mapping Close Up"/></td>
  </tr>
</table>

---

### 4. General Render Gallery
*Demonstrating complex mesh parsing, Constructive Solid Geometry (CSG), 4th-degree polynomial Toruses, procedural textures, and physical glass optics.*

<table align="center">
  <tr>
    <th align="center">BVH Multi-Primitive Stress Test (42 Logo Spheres)</th>
  </tr>
  <tr>
    <td align="center"><img src="https://github.com/user-attachments/assets/5426168f-28af-4005-b17a-3e68ceb81da8" alt="42 Logo Spheres"/></td>
  </tr>
  <tr>
    <th align="center">Refraction Physics (Ray Tracer Challenge Cover)</th>
  </tr>
  <tr>
    <td align="center"><img src="https://github.com/user-attachments/assets/e1a2c9a6-7738-4e30-b012-2e76f4aa4eb0" alt="Ray Tracer Challenge Book Cover"/></td>
  </tr>
  <tr>
    <th align="center">High-Poly OBJ Parsing (Stanford Dragons)</th>
  </tr>
  <tr>
    <td align="center"><img src="https://github.com/user-attachments/assets/049aedc8-ca49-4168-a346-98720fe2615d" alt="Stanford Dragons OBJ parsing"/></td>
  </tr>
  <tr>
    <th align="center">Constructive Solid Geometry (Atomium)</th>
  </tr>
  <tr>
    <td align="center"><img src="https://github.com/user-attachments/assets/2d0da57a-421d-4678-af4f-93c0d58c98c2" alt="Atomium Geometry"/></td>
  </tr>
  <tr>
    <th align="center">UV Mapped Chess & Procedural Textures</th>
  </tr>
  <tr>
    <td align="center"><img src="https://github.com/user-attachments/assets/e150f3fa-b1a0-40fd-a662-d4295331e506" alt="Chess and Procedural Textures"/></td>
  </tr>
  <tr>
    <th align="center">Groups, Mirrors & Procedural Textures (Rubik)</th>
  </tr>
  <tr>
    <td align="center"><img src="https://github.com/user-attachments/assets/80c09a0b-4602-4dce-b334-bd05c8fd2f9d" alt="Rubik Cubes"/></td>
  </tr>
  <tr>
    <th align="center">The Initial Feature Stress-Test</th>
  </tr>
  <tr>
    <td align="center"><img src="https://github.com/user-attachments/assets/50eefbd2-b45d-4b06-a21f-3654d6330f16" alt="Feature Testing Showcase"/></td>
  </tr>
</table>

---

## 🛠️ Instructions

### 📦 Compilation & Installation
Ensure you have a UNIX-based OS with a C compiler and the X11 libraries required for MiniLibX. Clone the repository and run the Makefile:
```bash
make
```

> **Note on Compilation:** Because this project natively integrates all advanced features (like the BVH, complex optics, and procedural textures) into a single unified architecture, the compilation is an all-in-one process. Running `make bonus` will build the exact same `./miniRT` executable as `make all`.

### 🧹 Cleaning & Utilities
* `make clean`: Removes the dynamically generated `obj/` directory containing all compiled object files (`.o`) and compiler dependency files (`.d`) for `miniRT`, while cleanly propagating the cleanup command to both the local `libft` and `MiniLibX` build paths.
* `make fclean`: Invokes the `clean` rule and subsequently deletes the compiled `miniRT` binary executable, completely removes the cloned `minilibx` repository directory, and purges the static library archive inside `libft`.
* `make re`: Performs a clean re-build from an absolute baseline by executing a full reset (`fclean`) followed immediately by a fresh, optimized compilation pass (`all`).
* **Memory Leak Validation (Valgrind):** Since `miniRT` functions as a performance-driven, multithreaded application via POSIX threads (`-pthread`), testing for memory allocations and open resources is executed directly on a target scene profile, completely eliminating the need for interactive shell UI suppressions:
  ```bash
  valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./miniRT scenes/working/solar_system.rt
  ```

### 💻 Usage
**1. Interactive BIOS Mode**
To browse available scenes, documentation, and controls without touching the terminal, run the program with no arguments:
```bash
./miniRT
```

**2. Direct Scene Rendering**
To instantly launch a specific scene, provide the path to a `.rt` file as the first argument.
```bash
./miniRT scenes/working/solar_system.rt
```

### 🎮 Real-Time Controls
While a scene is actively rendering, you can interact with the environment:
* **`Arrows`**: Translate camera (Up/Down/Left/Right).
* **`W/S`, `A/D`, `Q/E`**: Rotate camera (Pitch, Yaw, Roll).
* **`Mouse Scroll`**: Zoom in/out physically along the ray aimed precisely at your cursor coordinate.
* **`1, 2, 3, 4`**: Adjust Anti-Aliasing (1 = None, 2 = 4x, 3 = 9x, 4 = 16x).
* **`P`**: Toggle Pixelated Performance Mode (Subsamples the grid by 4x4 blocks to allow smooth real-time camera movement).
* **`F` / `M`**: Toggle Focal Blur / Toggle Temporal Motion Blur.
* **`ESC`**: Return to BIOS Menu / Exit cleanly.

---

### 🚨 The Norm
Moulinette relies on a program called `norminette` to check if your files comply with the 42 Norm. Every single `.c` and `.h` file must pass this check. If there is a norm error, you will receive a 0.

**The 42 Header:**
Before writing any code, every file must start with the standard 42 header. `norminette` will automatically fail any file missing this specific signature.
```c
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   miniRT.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 02:36:42 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/12 21:36:53 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
```

Run the following command in the root of your repository to check all your files at once:
```bash
norminette -R CheckForbiddenSourceHeader src/ inc/
```

---

## 📚 Resources & References

The mathematical physics and optical algorithms in this engine were made possible through extensive research from the following classic references:

**System Manuals:**
* `man 3 math` - The standard C math library programmer's manual. Essential for understanding compilation links (`-lm`) when dealing with the advanced mathematical formulas required for ray-primitive intersections and polynomials.
* `man 7 pthreads` / `man 3 pthread_create` / `man 3 pthread_join` / `man 3 pthread_mutex_init` / `man 3 pthread_mutex_lock` / `man 3 pthread_mutex_unlock` - System manuals detailing the POSIX threads API and mutex synchronization operations, crucial for safely implementing the multithreaded tile-based rendering pool and work-stealing architecture.
* `man 2 gettimeofday` - System manual for retrieving the system's clock time, an authorized external function. Essential for performance tracking, render time calculations, and benchmarking.
* `man ./<path_to_mlx>/man/man3/mlx.3` / `man ./<path_to_mlx>/man/man3/mlx_new_window` / `man ./<path_to_mlx>/man/man3/mlx_new_image` / `man ./<path_to_mlx>/man/man3/mlx_pixel_put` - Local manual pages provided by the 42 `minilibx-linux` package detailing the graphical API functions.
    * *Note: Because `minilibx` is a custom 42 library and not a standard Linux system library, typing `man mlx` natively will fail. However, you can read the documentation by pointing the `man` command directly to the raw files inside the `man/man3/` directory of the cloned `minilibx` folder.*

**Core Raytracing Theory:**
* **[The Ray Tracer Challenge (Jamis Buck)](https://z-library.im/book/GZa1MDL0Zl/the-ray-tracer-challenge.html):** A foundational, test-driven guide to building a 3D renderer from scratch. Covers core linear algebra, matrices, transformations, and the Blinn-Phong reflection model.
* **[Ray Tracing in One Weekend (Peter Shirley)](https://raytracing.github.io/books/RayTracingInOneWeekend.html):** The industry-standard introduction to CGI math, focusing on camera ray generation, basic primitive intersections, and anti-aliasing techniques.
* **[Ray Tracing: The Next Week (Peter Shirley)](https://raytracing.github.io/books/RayTracingTheNextWeek.html):** Expands on the basic ray tracer by introducing Bounding Volume Hierarchies (BVH), procedural Perlin noise, and emissive lighting.
* **[Ray Tracing: The Rest of Your Life (Peter Shirley)](https://raytracing.github.io/books/RayTracingTheRestOfYourLife.html):** A deep dive into the advanced mathematics of physically based rendering, focusing on light scattering and probability density.

**Advanced Features & Bonus Mechanics:**
* **[Texture Mapping Add-on (Ray Tracer Challenge)](http://www.raytracerchallenge.com/bonus/texture-mapping.html):** Supplementary guide covering the algorithms required to unwrap 2D coordinates and wrap textures around 3D primitives (Spherical, Planar, Cylindrical, and Cubical mapping).
* **[Bounding Boxes Add-on (Ray Tracer Challenge)](http://www.raytracerchallenge.com/bonus/bounding-boxes.html):** Details the implementation of Axis-Aligned Bounding Boxes (AABB) and spatial partitioning trees to drastically optimize intersection speeds for complex geometry.
* **[Area Lights Add-on (Ray Tracer Challenge)](http://www.raytracerchallenge.com/bonus/area-light.html):** Explains how to achieve physically accurate soft shadows (penumbrae) using Stratified Jittered Sampling across a simulated light grid.

**Advanced Ray Tracer Implementation Series (Ksatese - Medium):**
* **[Part 1: Foundational Setup](https://medium.com/@Ksatese/advanced-ray-tracer-part-1-7e0c015ace79):** Structuring the initial architecture, memory management, and baseline math operations.
* **[Part 2: Lighting & Shading](https://medium.com/@Ksatese/advanced-ray-tracer-part-2-f5313530581c):** Translating normal vectors to world space and applying ambient, diffuse, and specular calculations.
* **[Part 3: Shadows & Reflections](https://medium.com/@Ksatese/advanced-ray-tracer-part-3-d111808915cb):** Handling ray recursion limits, calculating surface bias to prevent shadow acne, and bouncing reflection rays.
* **[Part 4: Patterns & Textures](https://medium.com/@Ksatese/advanced-ray-tracer-part-4-87d1c98eecff):** Implementing transform matrices for patterns and generating procedural volumetric designs like stripes and checkers.
* **[Part 5: Refractions](http://medium.com/@Ksatese/advanced-ray-tracer-part-5-0b4e6d14ea8c):** Modeling the physics of transparent objects, Snell's Law, and the Schlick approximation for the Fresnel effect.
* **[Part 6: Bounding Volumes](https://medium.com/@Ksatese/advanced-ray-tracer-part-6-f7978842081f):** Strategies for encapsulating complex objects into bounding boxes to optimize multi-threaded render times.
* **[Part 7: CSG & Complex Meshes](https://medium.com/@Ksatese/advanced-ray-tracer-part-7-5fe95cd69a1a):** Implementing the boolean state machine for Constructive Solid Geometry (Union, Intersection, Difference) and managing OBJ polygon data.

**42 Standards:**
* **[42 Norm V4](https://cdn.intra.42.fr/pdf/pdf/96987/en.norm.pdf):** The strict coding standard for 42 C projects.
* **[Official 42 Norminette Repository](https://github.com/42School/norminette):** The open-source linter enforcing the strict 42 coding standard.

### 🤖 AI Usage & Transparency
In alignment with the 42 curriculum AI instructions, AI was utilized during the development of this project for the following specific tasks:
* **Mathematical & Architectural Refactoring:** While the core implementation was heavily based on the cited ray tracing literature, AI was used extensively to refactor complex calculations, optimize linear algebra routines, and elevate the rendering engine to an elite performance level.
* **Collaborative Debugging:** The development process was a highly collaborative loop. While AI assisted with drafting optimal mathematical implementations (like the Quartic solver and Jim Arvo's algorithm), we actively guided the architecture, pointed out logical faults, corrected AI-generated edge-case failures, and proposed structural solutions (such as the flat-array BVH and zero-malloc intersection loop) to ensure the engine remained robust and memory-safe.
* **Documentation & Formatting:** AI assisted in compiling this `README.md`, ensuring all highly complex internal algorithms were accurately described and formatted into an accessible, professional manual.
