/*
 * View frustum culling class
 *
 * Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
 *
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

#include <math.h>
#include <glm/glm.hpp>

namespace Engine
{
    namespace Util
    {
        class Frustum
        {
        public:
            enum class ESide
            {
                eLeft = 0,
                eRight = 1,
                eTop = 2,
                eBottom = 3,
                eBack = 4,
                eFront = 5
            };
            std::array<glm::vec4, 6> planes;

            void update(glm::mat4 matrix)
            {
                planes[ESide::eLeft].x = matrix[0].w + matrix[0].x;
                planes[ESide::eLeft].y = matrix[1].w + matrix[1].x;
                planes[ESide::eLeft].z = matrix[2].w + matrix[2].x;
                planes[ESide::eLeft].w = matrix[3].w + matrix[3].x;

                planes[ESide::eRight].x = matrix[0].w - matrix[0].x;
                planes[ESide::eRight].y = matrix[1].w - matrix[1].x;
                planes[ESide::eRight].z = matrix[2].w - matrix[2].x;
                planes[ESide::eRight].w = matrix[3].w - matrix[3].x;

                planes[ESide::eTop].x = matrix[0].w - matrix[0].y;
                planes[ESide::eTop].y = matrix[1].w - matrix[1].y;
                planes[ESide::eTop].z = matrix[2].w - matrix[2].y;
                planes[ESide::eTop].w = matrix[3].w - matrix[3].y;

                planes[ESide::eBottom].x = matrix[0].w + matrix[0].y;
                planes[ESide::eBottom].y = matrix[1].w + matrix[1].y;
                planes[ESide::eBottom].z = matrix[2].w + matrix[2].y;
                planes[ESide::eBottom].w = matrix[3].w + matrix[3].y;

                planes[ESide::eBack].x = matrix[0].w + matrix[0].z;
                planes[ESide::eBack].y = matrix[1].w + matrix[1].z;
                planes[ESide::eBack].z = matrix[2].w + matrix[2].z;
                planes[ESide::eBack].w = matrix[3].w + matrix[3].z;

                planes[ESide::eFront].x = matrix[0].w - matrix[0].z;
                planes[ESide::eFront].y = matrix[1].w - matrix[1].z;
                planes[ESide::eFront].z = matrix[2].w - matrix[2].z;
                planes[ESide::eFront].w = matrix[3].w - matrix[3].z;

                for (auto i = 0; i < planes.size(); i++)
                {
                    float length = sqrtf(planes[i].x * planes[i].x + planes[i].y * planes[i].y + planes[i].z * planes[i].z);
                    planes[i] /= length;
                }
            }

            bool checkSphere(glm::vec3 pos, float radius)
            {
                for (auto i = 0; i < planes.size(); i++)
                {
                    if ((planes[i].x * pos.x) + (planes[i].y * pos.y) + (planes[i].z * pos.z) + planes[i].w <= -radius)
                        return false;
                }
                return true;
            }
        };
    }
}