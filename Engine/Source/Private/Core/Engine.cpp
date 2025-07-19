#include <Core/Engine.h>

#include <iostream>
#include <algorithm>

#include <volk.h>

#include <Core/GlfwWindow.h>
#include <Core/ModelManager.h>
#include <Core/MeshInstance.h>
#include <Core/MeshPrimitive.h>
#include <Core/Mesh.h>
#include <Core/Camera.h>
#include <Core/Scene.h>
#include <Core/Vertex.h>
#include <Core/Transform.h>
#include <Core/RenderTarget.h>
#include <Core/Renderer.h>
#include <Vulkan/Device.h>
#include <Vulkan/DescriptorPool.h>
#include <Vulkan/DescriptorSetLayoutManager.h>
#include <Vulkan/GlobalDescriptorSetManager.h>
#include <Core/GlmRegistration.h>

inline float Wrap180(float angle)
{
	angle = std::fmod(angle + 180.0f, 360.0f);
	if (angle < 0.0f)
		angle += 360.0f;
	return angle - 180.0f;
}

inline glm::vec3 WrapEuler180(const glm::vec3 angles)
{
	return glm::vec3(Wrap180(angles.x), Wrap180(angles.y), Wrap180(angles.z));
}

inline float RoundDP(float value, int dp)
{
	float factor = (float)std::pow(10.0f, dp);
	return std::round(value * factor) / factor;
}

inline glm::vec3 RoundEulerDP(const glm::vec3& angles, int dp)
{
	return glm::vec3(RoundDP(angles.x, dp), RoundDP(angles.y, dp), RoundDP(angles.z, dp));
}

namespace Nightbird
{
	Engine::Engine()
	{
		GlmRegistration();

		if (volkInitialize() != VK_SUCCESS)
		{
			std::cerr << "Failed to initialize Volk" << std::endl;
		}

		glfwWindow = std::make_unique<GlfwWindow>();
		renderer = std::make_unique<Renderer>(glfwWindow.get());
		glfwWindow->SetRendererPointer(renderer.get());

		modelManager = std::make_unique<ModelManager>(renderer->GetDevice(), renderer->GetDescriptorSetLayoutManager()->GetMeshDescriptorSetLayout(), renderer->GetDescriptorSetLayoutManager()->GetMaterialDescriptorSetLayout(), renderer->GetDescriptorPool()->Get());

		scene = std::make_unique<Scene>(renderer->GetDevice(), modelManager.get(), renderer->GetGlobalDescriptorSetManager(), renderer->GetDescriptorPool()->Get());
	}

	Engine::~Engine()
	{

	}

	GlfwWindow* Engine::GetGlfwWindow() const
	{
		return glfwWindow.get();
	}

	Renderer* Engine::GetRenderer() const
	{
		return renderer.get();
	}

	Scene* Engine::GetScene() const
	{
		return scene.get();
	}

	ModelManager* Engine::GetModelManager() const
	{
		return modelManager.get();
	}

	void Engine::Run()
	{
		while (!glfwWindowShouldClose(glfwWindow->Get()))
		{
			glfwPollEvents();
			modelManager->ProcessUploadQueue();
			renderer->DrawFrame(scene.get());
		}
		vkDeviceWaitIdle(renderer->GetDevice()->GetLogical());
	}
}