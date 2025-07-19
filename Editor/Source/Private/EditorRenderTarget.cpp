#include <EditorRenderTarget.h>

#include <Core/Engine.h>
#include <Core/Renderer.h>
#include <Vulkan/Texture.h>
#include <ImGuiOverlay.h>
#include <SceneWindow.h>

#include <Vulkan/Instance.h>
#include <Vulkan/Device.h>
#include <Vulkan/SwapChain.h>
#include <Core/ModelManager.h>

namespace Nightbird
{
	EditorRenderTarget::EditorRenderTarget(Renderer* renderer, VulkanInstance* instance, VulkanDevice* device, VulkanSwapChain* swapChain, VulkanRenderPass* renderPass, GLFWwindow* glfwWindow, Scene* scene, ModelManager* modelManager)
		: RenderTarget(renderer)
	{
		imGuiOverlay = std::make_unique<VulkanImGuiOverlay>(instance, device, swapChain, renderPass, glfwWindow, scene, modelManager);
	}

	EditorRenderTarget::~EditorRenderTarget()
	{

	}
	
	void EditorRenderTarget::Render(Scene* scene, VulkanRenderPass* renderPass, VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkExtent2D extent)
	{
		SceneWindow* sceneWindow = static_cast<SceneWindow*>(imGuiOverlay->GetWindow("Scene Window"));
		if (sceneWindow)
		{
			if (sceneWindow->ShouldResize())
				sceneWindow->RecreateRenderResources();
			sceneWindow->GetColorTexture()->TransitionToColor(commandBuffer);
		}

		if (sceneWindow)
		{
			sceneWindow->BeginRenderPass(commandBuffer);
			renderer->DrawScene(scene, commandBuffer, sceneWindow->GetExtent());
			sceneWindow->EndRenderPass(commandBuffer);
		}

		if (sceneWindow)
			sceneWindow->GetColorTexture()->TransitionToShaderRead(commandBuffer);

		renderPass->Begin(commandBuffer, framebuffer, extent);
		imGuiOverlay->Render(commandBuffer);
		renderPass->End(commandBuffer);
	}
}