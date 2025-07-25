#include <Core/Mesh.h>

#include <iostream>
#include <chrono>

#include <Vulkan/Device.h>
#include <Vulkan/Texture.h>
#include <Core/MeshPrimitive.h>

#include <stb_image.h>

namespace Nightbird
{
	Mesh::Mesh(VulkanDevice* device, VkDescriptorSetLayout uniformDescriptorSetLayout)
		: device(device), uniformDescriptorSetLayout(uniformDescriptorSetLayout)
	{

	}

	Mesh::~Mesh()
	{

	}

	size_t Mesh::GetPrimitiveCount() const
	{
		return primitives.size();
	}

	MeshPrimitive* Mesh::GetPrimitive(size_t index) const
	{
		if (index < primitives.size() && primitives[index])
			return primitives[index].get();
		return nullptr;
	}

	VkDescriptorSetLayout Mesh::GetUniformDescriptorSetLayout() const
	{
		return uniformDescriptorSetLayout;
	}

	void Mesh::AddPrimitive(std::unique_ptr<MeshPrimitive> meshPrimitive)
	{
		primitives.push_back(std::move(meshPrimitive));
	}
}