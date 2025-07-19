#pragma once

#include <string>

#include <Core/SceneObject.h>
#include <Core/Transform.h>

namespace Nightbird
{
	class VulkanDevice;
	class VulkanUniformBuffer;
	class Mesh;

	class PrefabInstance : public SceneObject
	{
	public:
		using SceneObject::SceneObject;
		PrefabInstance(const char* name, const char* prefabPath = "");
		PrefabInstance(const std::string& name, const std::string& prefabPath = "");
		~PrefabInstance() override;
		
		const std::string& GetPrefabPath() const;

		void Serialize(json& out) const override;
		void Deserialize(const json& in) override;

	protected:
		std::string prefabPath;
		
		RTTR_ENABLE(Nightbird::SceneObject)
		RTTR_REGISTRATION_FRIEND
	};
}