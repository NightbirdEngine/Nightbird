#pragma once

#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <future>
#include <string>

#include <Vulkan/Texture.h>

#include <volk.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>

namespace Nightbird
{
	class VulkanDevice;
	class VulkanTexture;
	class Transform;
	class Mesh;
	class MeshInstance;
	struct MeshInfo;
	struct Model;
	
	class ModelManager
	{
	public:
		using LoadCallback = std::function<void(std::shared_ptr<Model>)>;

		ModelManager(VulkanDevice* device, VkDescriptorSetLayout uniformDescriptorSetLayout, VkDescriptorSetLayout materialDescriptorSetLayout, VkDescriptorPool descriptorPool);
		~ModelManager();

		const std::unordered_map<std::string, std::shared_ptr<Model>>& GetModels();
		
		std::shared_ptr<Model> GetModel(const std::string& path);
		
		std::shared_ptr<Model> LoadModel(const std::filesystem::path& path);
		void LoadModelAsync(const std::filesystem::path& path, LoadCallback callback = 0);

		void ProcessUploadQueue();

	private:
		VulkanDevice* device;
		
		VkDescriptorSetLayout uniformDescriptorSetLayout;
		VkDescriptorSetLayout materialDescriptorSetLayout;

		VkDescriptorPool descriptorPool;
		
		std::unordered_map<std::string, std::shared_ptr<Model>> models;

		std::unordered_map<std::string, std::future<std::shared_ptr<Model>>> modelFutures;

		std::mutex uploadQueueMutex;
		std::queue<std::shared_ptr<Model>> uploadQueue;

		std::unordered_map<std::string, LoadCallback> pendingCallbacks;

		std::shared_ptr<VulkanTexture> fallbackTexture;

		void LoadTextures(std::shared_ptr<Model>& model);
		
		std::shared_ptr<Model> LoadModelInternal(const std::filesystem::path& path);

		void UploadModel(std::shared_ptr<Model>& model);
		
		std::shared_ptr<VulkanTexture> CreateFallbackTexture(glm::vec4 color);
		
		bool DecodeImage(const fastgltf::Asset& asset, const fastgltf::Image& image, std::vector<uint8_t>& outPixels, int& outWidth, int& outHeight, int& outChannels);
	};
}