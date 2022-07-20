#pragma once

#ifdef VULKAN_API_SUPPORT
#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.hpp>
#endif

#ifdef OPENGL_API_SUPPORT
#endif

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_XYZW_ONLY

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#define _USE_MATH_DEFINES
#include <cmath>

#include <cassert>
#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <optional>
#include <numeric>
#include <random>
#include <functional>
#include <sstream>
#include <queue>
#include <variant>

#include <chrono>

#include <format>
#include <source_location>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>

#include <filesystem>
namespace fs = std::filesystem;

constexpr const uint32_t max_light_count = 32;

#include <util/ulog.hpp>
#include <util/memory.hpp>
#include <util/function.hpp>
#include <util/helpers.hpp>
#include <util/bimap.hpp>
#include "configuration.h"