#pragma once

#include <vector>

#include <ImGuiWindow.h>

namespace Nightbird
{
	class Scene;
	class VulkanImGuiOverlay;
	
	class CreateObjectWindow : public ImGuiWindow
	{
	public:
		CreateObjectWindow(Scene* scene, bool open = false);

	protected:
		void OnRender() override;

		Scene* m_Scene;
	};
}