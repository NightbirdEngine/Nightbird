#include <Core/Transform.h>

#include <iostream>

#include <rttr/registration.h>

namespace Nightbird
{
	Transform::Transform()
		: position(0.0f), rotation(), scale(1.0f)
	{

	}

	Transform::Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale)
		: position(position), rotation(rotation), scale(scale)
	{

	}

	Transform::~Transform()
	{

	}

	glm::mat4 Transform::GetLocalMatrix() const
	{
		glm::mat4 t = glm::translate(glm::mat4(1.0f), position);
		glm::mat4 r = glm::mat4_cast(rotation);
		glm::mat4 s = glm::scale(glm::mat4(1.0f), scale);
		return t * r * s;
	}
}

RTTR_REGISTRATION
{
	using namespace Nightbird;

	rttr::registration::class_<Transform>("Transform")
	.constructor<>()
	.property("Position", &Transform::position)
	.property("Rotation", &Transform::rotation)
	.property("Scale", &Transform::scale);
}