#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <rttr/registration.h>

void GlmRegistration()
{

}

RTTR_REGISTRATION
{
	rttr::registration::class_<glm::vec3>("vec3")
	.constructor<>()
	.property("x", &glm::vec3::x)
	.property("y", &glm::vec3::y)
	.property("z", &glm::vec3::z);

	rttr::registration::class_<glm::quat>("quat")
	.constructor<>()
	.property("x", &glm::quat::x)
	.property("y", &glm::quat::y)
	.property("z", &glm::quat::z)
	.property("w", &glm::quat::w);
}