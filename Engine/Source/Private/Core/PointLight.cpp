#include <Core/PointLight.h>

#include <iostream>

#include <Vulkan/Config.h>
#include <Vulkan/Device.h>
#include <Vulkan/UniformBuffer.h>
#include <Core/PointLightData.h>

namespace Nightbird
{
	PointLight::~PointLight()
	{

	}

	PointLightData PointLight::GetData() const
	{
		PointLightData data;
		data.positionRadius = glm::vec4(transform.position, radius);
		data.colorIntensity = glm::vec4(color, intensity);

		return data;
	}
}

RTTR_REGISTRATION
{
	rttr::registration::class_<Nightbird::PointLight>("PointLight")
	.constructor<std::string>()
	.property("Radius", &Nightbird::PointLight::radius)
	.property("Intensity", &Nightbird::PointLight::intensity)
	.property("Color", &Nightbird::PointLight::color);

	rttr::registration::method("CreatePointLight", [](const std::string& name) -> Nightbird::SceneObject*
	{
		return new Nightbird::PointLight(name);
	});
}