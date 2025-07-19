#pragma once

#include <Core/RenderTarget.h>

#include <ImGuiOverlay.h>

namespace Nightbird
{
	class VulkanInstance;
	class VulkanDevice;
	class VulkaVulkanSwapChain;
	class ModelManager;
	
	class EditorRenderTarget : public RenderTarget
	{
	public:
		EditorRenderTarget(Renderer* renderer, VulkanInstance* instance, VulkanDevice* device, VulkanSwapChain* swapChain, VulkanRenderPass* renderPass, GLFWwindow* glfwWindow, Scene* scene, ModelManager* modelManager);
		virtual ~EditorRenderTarget();
		
		void Render(Scene* scene, VulkanRenderPass* renderPass, VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkExtent2D extent) override;

	private:
		std::unique_ptr<VulkanImGuiOverlay> imGuiOverlay;
	};
}