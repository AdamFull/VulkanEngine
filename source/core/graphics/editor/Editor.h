#pragma once
#include "util/helpers.hpp"
#include "graphics/scene/objects/RenderObject.h"

namespace Engine
{
    namespace Core
    {
        class CEditor : public utl::singleton<CEditor>
        {
        public:
            void selectObject(std::shared_ptr<Scene::CRenderObject>& object);
            void selectObject(uint64_t objectId);
            void deselectObject(std::shared_ptr<Scene::CRenderObject>& object);
            void deselectObject(uint64_t objectId);
            void deselectAll();

            bool isSelected(std::shared_ptr<Scene::CRenderObject>& object);
            bool isSelected(uint64_t objectId);

            std::shared_ptr<Scene::CRenderObject> getLastSelection();
        private:
            std::unordered_map<std::string, std::shared_ptr<Scene::CRenderObject>> mSelectedObjects;
        };
    }
}