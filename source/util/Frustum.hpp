#pragma once
#include "helpers.hpp"
#include <glm/glm.hpp>

namespace Engine
{
    namespace Util
    {
        class CFrustum : public utl::singleton<CFrustum>
        {
        public:
            void update(const glm::mat4& view, const glm::mat4& projection);
            bool checkPoint(const glm::vec3& pos);
            bool checkSphere(const glm::vec3& pos, float radius);
            bool checkBox(const glm::vec3& start, const glm::vec3& end);
        private:
            void normSide(size_t side);
            std::array<std::array<float, 4>, 6> frustumSides{};
        };
    }
}