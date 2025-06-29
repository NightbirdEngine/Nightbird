#include <Core/Scene.h>

#include <iostream>

#include <Vulkan/Device.h>
#include <Vulkan/StorageBuffer.h>
#include <Vulkan/GlobalDescriptorSetManager.h>
#include <Core/SceneObject.h>
#include <Core/MeshInstance.h>
#include <Core/ModelManager.h>
#include <Core/Mesh.h>
#include <Core/Camera.h>
#include <Core/CameraUBO.h>
#include <Core/PointLight.h>
#include <Core/PointLightData.h>
#include <Core/Transform.h>
#include <Core/Model.h>

using namespace Nightbird;

Scene::Scene(VulkanDevice* device, ModelManager* modelManager, GlobalDescriptorSetManager* globalDescriptorSetManager, VkDescriptorSetLayout cameraDescriptorSetLayout, VkDescriptorPool descriptorPool)
	: device(device), modelManager(modelManager), globalDescriptorSetManager(globalDescriptorSetManager), cameraDescriptorSetLayout(cameraDescriptorSetLayout), descriptorPool(descriptorPool)
{

}

Scene::~Scene()
{

}

const std::vector<std::unique_ptr<SceneObject>>& Scene::GetObjects() const
{
	return objects;
}

std::vector<std::unique_ptr<SceneObject>>& Scene::GetObjectsMutable()
{
	return objects;
}

Camera* Scene::GetMainCamera() const
{
	return mainCamera;
}

void Scene::SetMainCamera(Camera* camera)
{
	mainCamera = camera;
}

SceneObject* Scene::CreateSceneObject(const std::string& name, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, Transform* parent)
{
	std::string instanceName = name;
	int counter = 1;
	while (objectNames.count(instanceName))
	{
		instanceName = name + std::to_string(counter);
		++counter;
	}
	objectNames.insert(instanceName);

	std::unique_ptr<SceneObject> object = std::make_unique<SceneObject>(instanceName);
	object->transform.position = position;
	object->transform.rotation = rotation;
	object->transform.scale = scale;
	object->transform.SetParent(parent);

	SceneObject* objectPtr = object.get();
	objects.push_back(std::move(object));

	return objectPtr;
}

Camera* Scene::CreateCamera(const std::string& name, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, Transform* parent)
{
	std::string cameraName = name;
	int counter = 1;
	while (objectNames.count(cameraName))
	{
		cameraName = name + std::to_string(counter);
		++counter;
	}
	objectNames.insert(cameraName);

	std::unique_ptr<Camera> camera = std::make_unique<Camera>(cameraName);
	camera->transform.position = position;
	camera->transform.rotation = rotation;
	camera->transform.scale = scale;
	camera->transform.SetParent(parent);

	Camera* cameraPtr = camera.get();
	objects.push_back(std::move(camera));

	if (!GetMainCamera())
	{
		SetMainCamera(cameraPtr);
	}
	
	return cameraPtr;
}

PointLight* Scene::CreatePointLight(const std::string& name, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, Transform* parent)
{
	std::string lightName = name;
	int counter = 1;
	while (objectNames.count(lightName))
	{
		lightName = name + std::to_string(counter);
		++counter;
	}
	objectNames.insert(lightName);

	std::unique_ptr<PointLight> light = std::make_unique<PointLight>(lightName);
	light->transform.position = position;
	light->transform.rotation = rotation;
	light->transform.scale = scale;
	light->transform.SetParent(parent);

	PointLight* cameraPtr = light.get();
	objects.push_back(std::move(light));

	return cameraPtr;
}

MeshInstance* Scene::CreateMeshInstance(const std::string& name, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, Transform* parent, std::shared_ptr<Mesh> mesh)
{
	std::string instanceName = name;
	int counter = 1;
	while (objectNames.count(instanceName))
	{
		instanceName = name + std::to_string(counter);
		++counter;
	}
	objectNames.insert(instanceName);
	
	std::unique_ptr<MeshInstance> meshInstance = std::make_unique<MeshInstance>(instanceName, mesh, device, descriptorPool);
	meshInstance->transform.position = position;
	meshInstance->transform.rotation = rotation;
	meshInstance->transform.scale = scale;
	meshInstance->transform.SetParent(parent);

	MeshInstance* meshInstancePtr = meshInstance.get();
	objects.push_back(std::move(meshInstance));
	
	return meshInstancePtr;
}

void Scene::InstantiateModelNode(const std::shared_ptr<Model>& model, const fastgltf::Node& node, Transform* parent)
{
	fastgltf::math::fvec3 gltfTranslation(0.0f, 0.0f, 0.0f);
	fastgltf::math::fquat gltfRotation;
	fastgltf::math::fvec3 gltfScale(1.0f, 1.0f, 1.0f);

	if (auto* transform = std::get_if<fastgltf::math::fmat4x4>(&node.transform))
	{
		fastgltf::math::decomposeTransformMatrix(*transform, gltfScale, gltfRotation, gltfTranslation);
	}
	else if (auto* trs = std::get_if<fastgltf::TRS>(&node.transform))
	{
		gltfTranslation = trs->translation;
		gltfRotation = trs->rotation;
		gltfScale = trs->scale;
	}
	
	glm::vec3 position = glm::vec3(gltfTranslation.x(), gltfTranslation.y(), gltfTranslation.z());
	glm::quat rotation = glm::quat(gltfRotation.w(), gltfRotation.x(), gltfRotation.y(), gltfRotation.z());
	glm::vec3 scale = glm::vec3(gltfScale.x(), gltfScale.y(), gltfScale.z());
	
	SceneObject* object = nullptr;

	if (node.meshIndex.has_value())
	{
		if (node.meshIndex.value() < model->meshes.size())
		{
			object = CreateMeshInstance(node.name.c_str(), position, rotation, scale, parent, model->meshes[node.meshIndex.value()]);
		}
	}
	else
	{
		object = CreateSceneObject(node.name.c_str(), position, rotation, scale, parent);
	}

	for (const auto& childNodeIndex : node.children)
	{
		const auto& childNode = model->gltfAsset.nodes[childNodeIndex];
		if (object)
			InstantiateModelNode(model, childNode, &object->transform);
	}
}

SceneObject* Scene::InstantiateModel(const std::string& name, const Transform& transform)
{
	const auto& model = modelManager->GetModel(name);

	if (!model)
	{
		std::cout << "Model " << name << " not found. Make sure model is loaded first." << std::endl;
		return nullptr;
	}

	const fastgltf::Scene& gltfScene = model->gltfAsset.scenes[0];

	SceneObject* root = CreateSceneObject(name, transform.position, transform.rotation, transform.scale, nullptr);

	for (size_t rootNodeIndex : gltfScene.nodeIndices)
	{
		InstantiateModelNode(model, model->gltfAsset.nodes[rootNodeIndex], &root->transform);
	}

	return nullptr;
}

void Scene::UpdateBuffers(int currentFrame, VkExtent2D swapChainExtent)
{
	std::vector<PointLightData> pointLightData;

	for (const auto& object : GetObjects())
	{
		if (auto* meshInstance = dynamic_cast<MeshInstance*>(object.get()))
		{
			meshInstance->UpdateUniformBuffer(currentFrame);
		}
		else if (auto* camera = dynamic_cast<Camera*>(object.get()))
		{
			globalDescriptorSetManager->UpdateCamera(currentFrame, camera->GetUBO(swapChainExtent));
		}
		else if (auto* light = dynamic_cast<PointLight*>(object.get()))
		{
			pointLightData.push_back(light->GetData());
		}
	}
	
	globalDescriptorSetManager->UpdatePointLights(currentFrame, pointLightData);
}