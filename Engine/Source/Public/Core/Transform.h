#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Nightbird
{
	class SceneObject;

	class Transform
	{
	public:
		Transform();
		Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);
		~Transform();
		
		glm::mat4 GetLocalMatrix() const;
		
		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;
	};
}