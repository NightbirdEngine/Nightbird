#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <volk.h>

#include <Core/SceneObject.h>
#include <Core/Transform.h>
#include <Vulkan/UniformBuffer.h>

namespace Nightbird
{
	struct CameraUBO;
	
	class Camera : public SceneObject
	{
	public:
		using SceneObject::SceneObject;
		~Camera() override = default;
		
		CameraUBO GetUBO(VkExtent2D swapChainExtent) const;
		
		float fov = 70.0f;
		
		RTTR_ENABLE(Nightbird::SceneObject)
	};
}