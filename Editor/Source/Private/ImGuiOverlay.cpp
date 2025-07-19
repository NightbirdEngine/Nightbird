#include <ImGuiOverlay.h>

#include <vector>
#include <iostream>

#include <Vulkan/Instance.h>
#include <Vulkan/Device.h>
#include <Vulkan/SwapChain.h>
#include <Vulkan/RenderPass.h>

#include <Core/MeshInstance.h>
#include <Core/Camera.h>
#include <Core/Scene.h>
#include <Core/ModelManager.h>

#include <ImGuiDescriptorPool.h>
#include <SceneOutliner.h>
#include <LoadModelWindow.h>
#include <CreateObjectWindow.h>
#include <InstantiateModelWindow.h>
#include <Inspector.h>
#include <AssetBrowser.h>
#include <SceneWindow.h>
#include <AboutWindow.h>

namespace Nightbird
{
	VulkanImGuiOverlay::VulkanImGuiOverlay(VulkanInstance* instance, VulkanDevice* device, VulkanSwapChain* swapChain, VulkanRenderPass* renderPass, GLFWwindow* glfwWindow, Scene* scene, ModelManager* modelManager)
		: m_Window(glfwWindow), m_Scene(scene)
	{
		m_DescriptorPool = std::make_unique<ImGuiDescriptorPool>(device);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& imGuiIO = ImGui::GetIO(); (void)imGuiIO;
		imGuiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		
		imGuiIO.Fonts->AddFontFromFileTTF("Assets/Fonts/RobotoFlex-Regular.ttf", 16.0f);
		
		ImGui_ImplGlfw_InitForVulkan(glfwWindow, true);

		ImGui_ImplVulkan_InitInfo imGuiInitInfo = {};
		imGuiInitInfo.ApiVersion = VK_API_VERSION_1_3;
		imGuiInitInfo.Instance = instance->Get();
		imGuiInitInfo.PhysicalDevice = device->GetPhysical();
		imGuiInitInfo.Device = device->GetLogical();
		imGuiInitInfo.QueueFamily = device->graphicsQueueFamily;
		imGuiInitInfo.Queue = device->graphicsQueue;
		imGuiInitInfo.DescriptorPool = m_DescriptorPool->Get();
		imGuiInitInfo.RenderPass = renderPass->Get();
		imGuiInitInfo.Subpass = 0;
		imGuiInitInfo.MinImageCount = swapChain->GetMinImageCount();
		imGuiInitInfo.ImageCount = swapChain->GetImageCount();
		imGuiInitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

		ImGui_ImplVulkan_Init(&imGuiInitInfo);

		ImGui_ImplVulkan_CreateFontsTexture();
		
		m_Windows["Scene Outliner"] = std::make_unique<SceneOutliner>(scene, this);
		m_Windows["Load Model Window"] = std::make_unique<LoadModelWindow>(modelManager);
		m_Windows["Instantiate Model Window"] = std::make_unique<InstantiateModelWindow>(modelManager, scene);
		m_Windows["Create Object Window"] = std::make_unique<CreateObjectWindow>(scene);
		m_Windows["Inspector"] = std::make_unique<Inspector>(scene, this);
		m_Windows["Asset Browser"] = std::make_unique<AssetBrowser>(scene, this);
		m_Windows["Scene Window"] = std::make_unique<SceneWindow>(device, swapChain->GetColorFormat(), swapChain->GetDepthFormat());
		m_Windows["About"] = std::make_unique<AboutWindow>();
	}
	
	VulkanImGuiOverlay::~VulkanImGuiOverlay()
	{
		ImGui_ImplVulkan_DestroyFontsTexture();

		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	ImGuiWindow* VulkanImGuiOverlay::GetWindow(const std::string& title)
	{
		if (m_Windows.count(title))
		{
			return m_Windows[title].get();
		}
		return nullptr;
	}
	
	SceneObject* VulkanImGuiOverlay::GetSelectedObject() const
	{
		return m_SelectedObject;
	}

	void VulkanImGuiOverlay::SelectObject(SceneObject* object)
	{
		m_SelectedObject = object;
	}
	
	void VulkanImGuiOverlay::Render(VkCommandBuffer commandBuffer)
	{
		NewFrame();

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save Scene (Binary)"))
				{
					std::string path = "Assets/Scenes/Scene.scene";
					m_Scene->SaveSceneBIN(path);
				}
				if (ImGui::MenuItem("Save Scene (Text)"))
				{
					std::string path = "Assets/Scenes/Scene.tscene";
					m_Scene->SaveSceneJSON(path);
				}
				if (ImGui::MenuItem("Quit"))
					glfwSetWindowShouldClose(m_Window, true);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::MenuItem("Scene Window"))
				{
					if (m_Windows.count("Scene Window"))
						m_Windows["Scene Window"]->SetOpen(true);
				}
				if (ImGui::MenuItem("Scene Outliner"))
				{
					if (m_Windows.count("Scene Outliner"))
						m_Windows["Scene Outliner"]->SetOpen(true);
				}
				if (ImGui::MenuItem("Inspector"))
				{
					if (m_Windows.count("Inspector"))
						m_Windows["Inspector"]->SetOpen(true);
				}
				if (ImGui::MenuItem("Asset Browser"))
				{
					if (m_Windows.count("Asset Browser"))
						m_Windows["Asset Browser"]->SetOpen(true);
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("About"))
				{
					if (m_Windows.count("About"))
						m_Windows["About"]->SetOpen(true);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		for (auto& [_, window] : m_Windows)
		{
			window->Render();
		}
		
		Draw(commandBuffer);
	}

	void VulkanImGuiOverlay::OpenWindow(const std::string& title)
	{
		if (m_Windows.count(title))
			m_Windows[title]->SetOpen(true);
	}
	
	void VulkanImGuiOverlay::NewFrame()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		
		ImGuiStyle& style = ImGui::GetStyle();
		
		style.TabRounding = 4.0f;
		style.FrameRounding = 4.0f;
		style.WindowRounding = 6.0f;

		style.WindowPadding = ImVec2(8.0f, 8.0f);
		style.FramePadding = ImVec2(8.0f, 6.0f);
		style.ItemSpacing = ImVec2(10.0f, 8.0f);

		style.TabBarBorderSize = 0.0f;
		style.WindowBorderSize = 0.0f;
	}
	
	void VulkanImGuiOverlay::Draw(VkCommandBuffer commandBuffer)
	{
		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
	}
}