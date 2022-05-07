#pragma once
#include "helpers.hpp"
#include <glm/glm.hpp>

namespace Engine
{
    namespace Util
    {
        /**
         * @brief CPU frustum implementation
         * 
         */
        class CFrustum : public utl::singleton<CFrustum>
        {
        public:
            /**
             * @brief Updating camera frustum
             * 
             * @param view Camera view matrix
             * @param projection Camera projection matrix
             */
            void update(const glm::mat4& view, const glm::mat4& projection);

            /**
             * @brief Check is point in frustum
             * 
             * @param pos Point position
             * @return true when point is in frustum
             */
            bool checkPoint(const glm::vec3& pos);

            /**
             * @brief Check is sphere intersects with frustum
             * 
             * @param pos Sphere center position
             * @param radius Sphere radius
             * @return true when sphere intersects with camera frustum
             */
            bool checkSphere(const glm::vec3& pos, float radius);

            /**
             * @brief Check is bounding box intersects with frustum
             * 
             * @param start Bounding box start point
             * @param end Bounding box end boint
             * @return true when bounding box intersects with camera frustum
             */
            bool checkBox(const glm::vec3& start, const glm::vec3& end);
        private:
            void normSide(size_t side);
            std::array<std::array<float, 4>, 6> frustumSides{};
        };
    }
}