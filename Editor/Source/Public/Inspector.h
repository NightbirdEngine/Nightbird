#pragma once

#include <ImGuiWindow.h>

#include <rttr/type.h>
#include <rttr/registration.h>

namespace Nightbird
{
	class VulkanImGuiOverlay;
	class SceneObject;
	class Scene;

	class Inspector : public ImGuiWindow
	{
	public:
		Inspector(Scene* scene, VulkanImGuiOverlay* overlay, bool open = true);

	protected:
		void OnRender() override;
		
		void RenderProperties(rttr::instance instance);
		
		Scene* m_Scene = nullptr;
		VulkanImGuiOverlay* m_Overlay = nullptr;

		SceneObject* m_SelectedObject = nullptr;
	};
}