#pragma once

#define GLFW_INCLUDE_VULKAN
#define APP_NAME "Vulkan App"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#define STB_IMAGE_STATIC

#define _USE_MATH_DEFINES
#include <cmath>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <external/tinygltf/json.hpp>

#include <util/uuid.hpp>
#include <util/memory.hpp>
#include <util/function.hpp>
#include <util/helpers.hpp>
#include <util/ulog.hpp>
#include <util/threading.hpp>
#include <util/DynamicNode.hpp>
#include <util/StringHelper.h>

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
#include <random>
#include <functional>

#include <chrono>

#include <filesystem>
namespace fs = std::filesystem;