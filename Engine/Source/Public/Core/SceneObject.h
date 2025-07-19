#pragma once

#include <vector>
#include <memory>
#include <string>

#include <Core/Transform.h>

#include <nlohmann/json.hpp>

#include <rttr/type>
#include <rttr/registration.h>
#include <rttr/registration_friend.h>

using json = nlohmann::json;

namespace Nightbird
{
	class SceneObject
	{
	public:
		SceneObject(const char* name);
		SceneObject(const std::string& name);
		virtual ~SceneObject() = default;

		const std::string& GetName() const;
		std::string GetPath() const;
		
		void SetParent(SceneObject* transform);
		SceneObject* GetParent() const;
		
		const std::vector<std::unique_ptr<SceneObject>>& GetChildren() const;

		glm::mat4 GetLocalMatrix() const;
		glm::mat4 GetWorldMatrix() const;

		void AddChild(std::unique_ptr<SceneObject> child);
		std::unique_ptr<SceneObject> DetachChild(SceneObject* child);
		
		virtual void Serialize(json& out) const;
		virtual void Deserialize(const json& in);
		
		Transform transform;

		SceneObject* parent = nullptr;
		
		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND

	protected:
		std::string name;
		
		std::vector<std::unique_ptr<SceneObject>> children;

		void SerializeBase(json& out) const;
		void DeserializeBase(const json& out);
	};
}