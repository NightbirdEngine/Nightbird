#pragma once

#include <memory>
#include <vector>
#include <string>

#include <volk.h>

#include <Core/SceneObject.h>
#include <Core/Transform.h>

namespace Nightbird
{
	class VulkanDevice;
	class VulkanUniformBuffer;
	class Mesh;
	
	class MeshInstance : public SceneObject
	{
	public:
		MeshInstance(const std::string& name, std::shared_ptr<Mesh> mesh, VulkanDevice* device, VkDescriptorPool descriptorPool);
		~MeshInstance() override;

		std::shared_ptr<const Mesh> GetMesh() const;
		const std::vector<VkDescriptorSet>& GetUniformDescriptorSets() const;
		
		void UpdateUniformBuffer(uint32_t currentImage);

	protected:
		VulkanDevice* device;

		std::vector<VkDescriptorSet> uniformDescriptorSets;

		std::vector<VulkanUniformBuffer> uniformBuffers;
		
		std::shared_ptr<Mesh> mesh;

		void CreateUniformBuffers();
		void CreateUniformDescriptorSets(VkDescriptorPool descriptorPool);
	};
}