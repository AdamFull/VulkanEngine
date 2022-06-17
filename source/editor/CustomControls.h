#pragma once
#include "util/Transform.hpp"

namespace engine
{
    struct FControls
    {
        static bool TextInput(const std::string& label, std::string& dest);
        static bool CheckBox(const std::string& label, bool* value);
        static bool ColorEdit3(const std::string& label, glm::vec3& value);
        static bool ColorEdit4(const std::string& label, glm::vec4& value);
        static bool DragFloat(const std::string& label, float* value, float step = 0.01f, float min = 0.0f, float max = 0.0f);
        static bool DragFloatVec3(const std::string& label, glm::vec3& values, float step = 0.01f, float min = 0.0f, float max = 0.0f);
        static bool DragTransform(FTransform& value, float step = 0.01f, float min = 0.0f, float max = 0.0f);
    };
}