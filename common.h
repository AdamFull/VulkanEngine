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

#include <chrono>

#include <format>
#include <source_location>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>

#include <filesystem>
namespace fs = std::filesystem;