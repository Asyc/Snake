# Snake
Snake Game Recreation using Vulkan.

### Dependencies:
* [GLFW](https://github.com/glfw/glfw) (Submodule)
* [VulkanSDK](https://vulkan.lunarg.com/sdk/home)

### Compilation:
Clone this repo recursively, and then you can generate build files with CMake.

#### Instructions:
1. Install the [VulkanSDK](https://vulkan.lunarg.com/sdk/home)
1. Clone this repo, with submodules:
   ```
    $ git clone --recurse-submodules https://github.com/Asyc/Snake.git
   ```
1. In the project root, create a new output directory.
1. Run CMake to generate build files, inside the output directory.
   ```
   $ cmake ../
   ```
1. Use the generated build files to compile an executable