#pragma once

#define GLFW_INCLUDE_VULKAN
#define APP_NAME "Vulkan App"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#define _USE_MATH_DEFINES
#include <cmath>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <iostream>
#include <fstream>
#include <string>
