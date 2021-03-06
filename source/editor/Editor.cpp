#include "Editor.h"

using namespace engine::core;
using namespace engine::editor;
using namespace engine::core::scene;

void CEditor::selectObject(utl::ref_ptr<CRenderObject>& object)
{
    if(!isSelected(object))
        mSelectedObjects.emplace(object->getName(), object);
}

void CEditor::selectObject(uint64_t objectId)
{

}

void CEditor::deselectObject(utl::ref_ptr<CRenderObject>& object)
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

bool CEditor::isSelected(utl::ref_ptr<CRenderObject>& object)
{
    const auto& it = mSelectedObjects.find(object->getName());
    return it != mSelectedObjects.end();
}

bool CEditor::isSelected(uint64_t objectId)
{
    return false;
}

utl::ref_ptr<CRenderObject> CEditor::getLastSelection()
{
    if(!mSelectedObjects.empty())
        return (--mSelectedObjects.end())->second;
    return nullptr;
}