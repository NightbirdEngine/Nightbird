#include <Vulkan/Pipeline.h>

#include <iostream>
#include <array>

#include <glm/glm.hpp>

#include <Core/Shader.h>
#include <Core/Vertex.h>
#include <Core/MeshInstance.h>
#include <Core/Mesh.h>
#include <Core/MeshPrimitive.h>
#include <Core/Camera.h>
#include <Vulkan/Device.h>
#include <Vulkan/SwapChain.h>
#include <Vulkan/RenderPass.h>
#include <Vulkan/DescriptorSetLayoutManager.h>
#include <Vulkan/GlobalDescriptorSetManager.h>

namespace Nightbird
{
	VulkanPipeline::VulkanPipeline(VulkanDevice* device, VulkanRenderPass* renderPass, VulkanDescriptorSetLayoutManager* descriptorSetLayoutManager, GlobalDescriptorSetManager* globalDescriptorSetManager, PipelineType type)
		: device(device), renderPass(renderPass), globalDescriptorSetManager(globalDescriptorSetManager), type(type)
	{
		CreateGraphicsPipeline(descriptorSetLayoutManager);
	}

	VulkanPipeline::~VulkanPipeline()
	{
		vkDestroyPipeline(device->GetLogical(), pipeline, nullptr);
		vkDestroyPipelineLayout(device->GetLogical(), pipelineLayout, nullptr);
	}

	void VulkanPipeline::SetDescriptorPool(VkDescriptorPool pool)
	{
		descriptorPool = pool;
	}

	void VulkanPipeline::CreateGraphicsPipeline(VulkanDescriptorSetLayoutManager* descriptorSetLayoutManager)
	{
		Shader vertShader(device->GetLogical(), "Assets/Shaders/Vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
		Shader fragShader(device->GetLogical(), "Assets/Shaders/Frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

		VkPipelineShaderStageCreateInfo shaderStages[] =
		{
			vertShader.GetStageCreateInfo(),
			fragShader.GetStageCreateInfo()
		};

		std::vector<VkDynamicState> dynamicStates =
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
		dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicStateInfo.pDynamicStates = dynamicStates.data();

		auto bindingDescription = Vertex::GetBindingDescription();
		auto attributeDescriptions = Vertex::GetAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
		inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo viewportStateInfo{};
		viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateInfo.viewportCount = 1;
		viewportStateInfo.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizationStateInfo{};
		rasterizationStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationStateInfo.depthClampEnable = VK_FALSE;
		rasterizationStateInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizationStateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizationStateInfo.lineWidth = 1.0f;
		rasterizationStateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizationStateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizationStateInfo.depthBiasEnable = VK_FALSE;
		rasterizationStateInfo.depthBiasConstantFactor = 0.0f;
		rasterizationStateInfo.depthBiasClamp = 0.0f;
		rasterizationStateInfo.depthBiasSlopeFactor = 0.0f;

		VkPipelineMultisampleStateCreateInfo multisamplingStateInfo{};
		multisamplingStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisamplingStateInfo.sampleShadingEnable = VK_FALSE;
		multisamplingStateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisamplingStateInfo.minSampleShading = 1.0f;
		multisamplingStateInfo.pSampleMask = nullptr;
		multisamplingStateInfo.alphaToCoverageEnable = VK_FALSE;
		multisamplingStateInfo.alphaToOneEnable = VK_FALSE;

		VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
		depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilInfo.depthTestEnable = VK_TRUE;
		depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		depthStencilInfo.minDepthBounds = 0.0f;
		depthStencilInfo.maxDepthBounds = 1.0f;
		depthStencilInfo.stencilTestEnable = VK_FALSE;
		depthStencilInfo.front = {};
		depthStencilInfo.back = {};

		if (type == PipelineType::Opaque)
		{
			depthStencilInfo.depthWriteEnable = VK_TRUE;
		}
		else if (type == PipelineType::Transparent)
		{
			depthStencilInfo.depthWriteEnable = VK_FALSE;
		}

		VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
		colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		if (type == PipelineType::Opaque)
		{
			colorBlendAttachmentState.blendEnable = VK_FALSE;
		}
		else if (type == PipelineType::Transparent)
		{
			colorBlendAttachmentState.blendEnable = VK_TRUE;
			colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
		}

		VkPipelineColorBlendStateCreateInfo colorBlendStateInfo{};
		colorBlendStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendStateInfo.logicOpEnable = VK_FALSE;
		colorBlendStateInfo.logicOp = VK_LOGIC_OP_COPY;
		colorBlendStateInfo.attachmentCount = 1;
		colorBlendStateInfo.pAttachments = &colorBlendAttachmentState;
		colorBlendStateInfo.blendConstants[0] = 0.0f;
		colorBlendStateInfo.blendConstants[1] = 0.0f;
		colorBlendStateInfo.blendConstants[2] = 0.0f;
		colorBlendStateInfo.blendConstants[3] = 0.0f;

		std::array<VkDescriptorSetLayout, 3> descriptorSetLayouts =
		{
			descriptorSetLayoutManager->GetGlobalDescriptorSetLayout(),
			descriptorSetLayoutManager->GetMeshDescriptorSetLayout(),
			descriptorSetLayoutManager->GetMaterialDescriptorSetLayout()
		};

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = (uint32_t)descriptorSetLayouts.size();
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(device->GetLogical(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			std::cerr << "Failed to create pipeline layout" << std::endl;
		}

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pViewportState = &viewportStateInfo;
		pipelineInfo.pRasterizationState = &rasterizationStateInfo;
		pipelineInfo.pMultisampleState = &multisamplingStateInfo;
		pipelineInfo.pDepthStencilState = &depthStencilInfo;
		pipelineInfo.pColorBlendState = &colorBlendStateInfo;
		pipelineInfo.pDynamicState = &dynamicStateInfo;
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = renderPass->Get();
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		if (vkCreateGraphicsPipelines(device->GetLogical(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
		{
			std::cerr << "Failed to create graphics pipeline" << std::endl;
		}
	}

	void VulkanPipeline::Render(VkCommandBuffer commandBuffer, uint32_t currentFrame, const std::vector<MeshInstance*>& meshInstances, Camera* camera)
	{
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		
		for (const auto& meshInstance : meshInstances)
		{
			std::shared_ptr<const Mesh> mesh = meshInstance->GetMesh();

			for (size_t i = 0; i < mesh->GetPrimitiveCount(); ++i)
			{
				MeshPrimitive* primitive = mesh->GetPrimitive(i);

				VkBuffer vertexBuffers[] = {primitive->vertexBuffer->Get()};
				VkDeviceSize offsets[] = {0};

				// Bind vertex and index buffers of mesh
				vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
				vkCmdBindIndexBuffer(commandBuffer, primitive->indexBuffer->Get(), 0, VK_INDEX_TYPE_UINT16);

				// Bind camera (view & proj matrices) and mesh (model matrix & textures) descriptor sets
				std::array<VkDescriptorSet, 3> descriptorSets = {globalDescriptorSetManager->GetDescriptorSets()[currentFrame], meshInstance->GetUniformDescriptorSets()[currentFrame], primitive->GetMaterialDescriptorSets()[currentFrame]};
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data(), 0, nullptr);

				// Draw the mesh
				vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(primitive->GetIndicesSize()), 1, 0, 0, 0);
			}
		}
	}
}