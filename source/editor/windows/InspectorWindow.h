#pragma once
#include "WindowBase.h"

namespace engine
{
	namespace core
	{
		namespace scene 
		{ 
			class CMeshComponent;
			class CCameraComponent; 
			class CLightComponent; 
		}
	}

	namespace editor
	{
		class CInspectorWindow : public CWindowBase
		{
		public:
			CInspectorWindow() = default;
			void draw() override;
		private:
			void processMesh(utl::ref_ptr<core::scene::CMeshComponent>& component);
			void processCamera(utl::ref_ptr<core::scene::CCameraComponent>& component);
			void processLight(utl::ref_ptr<core::scene::CLightComponent>& component);

			template<class _Ty>
			void processComponent(_Ty& component)
			{}

			template<>
			void processComponent<utl::ref_ptr<core::scene::CMeshComponent>>(utl::ref_ptr<core::scene::CMeshComponent>& component)
			{
				processMesh(component);
			}

			template<>
			void processComponent<utl::ref_ptr<core::scene::CCameraComponent>>(utl::ref_ptr<core::scene::CCameraComponent>& component)
			{
				processCamera(component);
			}

			template<>
			void processComponent<utl::ref_ptr<core::scene::CLightComponent>>(utl::ref_ptr<core::scene::CLightComponent>& component)
			{
				processLight(component);
			}
		};
	}
}