#include "Editor.h"

using namespace Engine::Core;
using namespace Engine::Core::Scene;

template<>
scope_ptr<CEditor> utl::singleton<CEditor>::_instance{nullptr};

void CEditor::selectObject(ref_ptr<CRenderObject>& object)
{
    if(!isSelected(object))
        mSelectedObjects.emplace(object->getName(), object);
}

void CEditor::selectObject(uint64_t objectId)
{

}

void CEditor::deselectObject(ref_ptr<CRenderObject>& object)
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

bool CEditor::isSelected(ref_ptr<CRenderObject>& object)
{
    const auto& it = mSelectedObjects.find(object->getName());
    return it != mSelectedObjects.end();
}

bool CEditor::isSelected(uint64_t objectId)
{
    return false;
}

ref_ptr<CRenderObject> CEditor::getLastSelection()
{
    if(!mSelectedObjects.empty())
        return (--mSelectedObjects.end())->second;
    return nullptr;
}