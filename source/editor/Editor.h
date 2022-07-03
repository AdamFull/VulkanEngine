#pragma once
#include "util/helpers.hpp"
#include "graphics/scene/objects/RenderObject.h"

namespace engine
{
	namespace editor
	{
		class CEditor : public utl::singleton<CEditor>
		{
		public:
			void selectObject(utl::ref_ptr<core::scene::CRenderObject> &object);
			void selectObject(uint64_t objectId);
			void deselectObject(utl::ref_ptr<core::scene::CRenderObject> &object);
			void deselectObject(uint64_t objectId);
			void deselectAll();

			bool isSelected(utl::ref_ptr<core::scene::CRenderObject> &object);
			bool isSelected(uint64_t objectId);

			utl::ref_ptr<core::scene::CRenderObject> getLastSelection();

		private:
			std::unordered_map<std::string, utl::ref_ptr<core::scene::CRenderObject>> mSelectedObjects;
		};
	}
}