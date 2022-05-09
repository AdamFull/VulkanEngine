#include "Editor.h"

using namespace Engine::Core;
using namespace Engine::Core::Scene;

template<>
std::unique_ptr<CEditor> utl::singleton<CEditor>::_instance{nullptr};

void CEditor::selectObject(std::shared_ptr<CRenderObject>& object)
{
    if(!isSelected(object))
        mSelectedObjects.emplace(object->getName(), object);
}

void CEditor::selectObject(uint64_t objectId)
{

}

void CEditor::deselectObject(std::shared_ptr<CRenderObject>& object)
{
    const auto& it = mSelectedObjects.find(object->getName());
    mSelectedObjects.erase(it);
}

void CEditor::deselectObject(uint64_t objectId)
{

}

void CEditor::deselectAll()
{
    mSelectedObjects.clear();
}

bool CEditor::isSelected(std::shared_ptr<CRenderObject>& object)
{
    const auto& it = mSelectedObjects.find(object->getName());
    return it != mSelectedObjects.end();
}

bool CEditor::isSelected(uint64_t objectId)
{
    return false;
}

std::shared_ptr<Scene::CRenderObject> CEditor::getLastSelection()
{
    if(!mSelectedObjects.empty())
        return (--mSelectedObjects.end())->second;
    return nullptr;
}