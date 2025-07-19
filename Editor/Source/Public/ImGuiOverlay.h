#pragma once

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

#include <ImGuiWindow.h>

#include <volk.h>
#include <glfw/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

namespace Nightbird
{
	class VulkanInstance;
	class VulkanDevice;
	class VulkanSwapChain;
	class VulkanRenderPass;
	class ImGuiDescriptorPool;
	class SceneObject;
	class Scene;
	class ModelManager;
	
	class VulkanImGuiOverlay
	{
	public:
		VulkanImGuiOverlay(VulkanInstance* instance, VulkanDevice* device, VulkanSwapChain* swapChain, VulkanRenderPass* renderPass, GLFWwindow* glfwWindow, Scene* scene, ModelManager* modelManager);
		~VulkanImGuiOverlay();
		
		ImGuiWindow* GetWindow(const std::string& title);
		
		SceneObject* GetSelectedObject() const;
		void SelectObject(SceneObject* object);
		
		void Render(VkCommandBuffer commandBuffer);
		
		void OpenWindow(const std::string& title);

	private:
		GLFWwindow* m_Window;

		std::unique_ptr<ImGuiDescriptorPool> m_DescriptorPool;

		Scene* m_Scene = nullptr;
		SceneObject* m_SelectedObject = nullptr;

		std::unordered_map<std::string, std::unique_ptr<ImGuiWindow>> m_Windows;
		
		void NewFrame();
		void Draw(VkCommandBuffer commandBuffer);
	};
}