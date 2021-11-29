#include "Transform.h"

using namespace Engine::Core::Scene::Objects;

glm::mat4 FTransform::GetModel()
{
    glm::mat4 translation{1.0};
    translation = glm::translate(translation, pos);
    //glm::radians(rot.x)
    if(rot.x != 0)
        translation = glm::rotate(translation, rot.x, glm::vec3(1.0, 0.0, 0.0));
    if(rot.y != 0)
        translation = glm::rotate(translation, rot.y, glm::vec3(0.0, 1.0, 0.0));
    if(rot.z != 0)
        translation = glm::rotate(translation, rot.z, glm::vec3(0.0, 0.0, 1.0));
    
    translation = glm::scale(translation, scale);

    return translation;
}

glm::mat4 FTransform::GetNormal()
{
    return glm::transpose(GetModel());
}