#pragma once

#ifdef VULKAN_API_SUPPORT
#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.hpp>
#endif

#ifdef OPENGL_API_SUPPORT
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

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

constexpr const uint32_t max_light_count = 32;

template<class _Ty>
using scope_ptr = std::unique_ptr<_Ty>;

template<class _Ty, class... _Args>
constexpr scope_ptr<_Ty> make_scope(_Args&& ...args)
{
	return std::make_unique<_Ty>(std::forward<_Args>(args)...);
}

template<class _Ty>
using ref_ptr = std::shared_ptr<_Ty>;

template<class _Ty, class... _Args>
constexpr ref_ptr<_Ty> make_ref(_Args&& ...args)
{
	return std::make_shared<_Ty>(std::forward<_Args>(args)...);
}

#include <util/function.hpp>
#include <util/helpers.hpp>
#include <util/ulog.hpp>
#include <util/bimap.hpp>