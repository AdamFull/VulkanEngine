#pragma once

#define GLFW_INCLUDE_VULKAN
#define APP_NAME "Vulkan App"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#define _USE_MATH_DEFINES
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "external/gltf/stb_image.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "external/gltf/tiny_gltf.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "external/tiny_obj_loader.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include "external/EasyDelegate/EasyDelegateImpl.hpp"

#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <optional>
#include <numeric>

#include <chrono>
