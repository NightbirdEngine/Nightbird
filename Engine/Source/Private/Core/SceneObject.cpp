#include <Core/SceneObject.h>

#include <iostream>

#include <Core/RTTRSerialization.h>

namespace Nightbird
{
	SceneObject::SceneObject(const char* name)
		: name(name ? name : "")
	{

	}

	SceneObject::SceneObject(const std::string& name)
		: name(name)
	{

	}
	
	const std::string& SceneObject::GetName() const
	{
		return name;
	}

	std::string SceneObject::GetPath() const
	{
		if (!parent || !parent->parent)
			return name;

		return parent->GetPath() + '/' + name;
	}

	void SceneObject::SetParent(SceneObject* newParent)
	{
		if (parent == newParent)
			return;
		
		std::unique_ptr<SceneObject> uniquePtr = nullptr;
		if (parent)
			uniquePtr = parent->DetachChild(this);

		parent = newParent;

		if (newParent && uniquePtr)
			newParent->AddChild(std::move(uniquePtr));
	}

	SceneObject* SceneObject::GetParent() const
	{
		return parent;
	}

	const std::vector<std::unique_ptr<SceneObject>>& SceneObject::GetChildren() const
	{
		return children;
	}

	glm::mat4 SceneObject::GetLocalMatrix() const
	{
		return transform.GetLocalMatrix();
	}

	glm::mat4 SceneObject::GetWorldMatrix() const
	{
		if (parent)
			return parent->GetWorldMatrix() * transform.GetLocalMatrix();
		else
			return transform.GetLocalMatrix();
	}

	void SceneObject::AddChild(std::unique_ptr<SceneObject> child)
	{
		child->parent = this;
		children.push_back(std::move(child));
	}

	std::unique_ptr<SceneObject> SceneObject::DetachChild(SceneObject* child)
	{
		auto it = std::find_if(children.begin(), children.end(),
			[child](const std::unique_ptr<SceneObject>& existingChild)
			{
				return existingChild.get() == child;
			}
		);

		if (it != children.end())
		{
			std::unique_ptr<SceneObject> detachedChild = std::move(*it);
			children.erase(it);
			detachedChild->parent = nullptr;
			return detachedChild;
		}

		return nullptr;
	}
	
	void SceneObject::Serialize(json& out) const
	{
		SerializeBase(out);
		
		out["__children"] = json::array();
		for (const auto& child : children)
		{
			json childJson;
			child->Serialize(childJson);
			out["__children"].push_back(childJson);
		}
	}

	void SceneObject::Deserialize(const json& in)
	{
		DeserializeBase(in);

		children.clear();
		if (in.contains("__children"))
		{
			for (const auto& childJson : in.at("__children"))
			{
				if (!childJson.contains("__type"))
				{
					std::cerr << "SceneObject Deserialize Error: Missing __type" << std::endl;
					continue;
				}
				
				std::string childTypeName = childJson["__type"];
				rttr::type childType = rttr::type::get_by_name(childTypeName);
				
				if (!childType.is_valid())
				{
					std::cerr << "SceneObject Deserialize Error: Unknown type: " << childTypeName << std::endl;
					continue;
				}
				
				auto create = rttr::type::get_global_method("Create" + childTypeName);
				if (create.is_valid())
				{
					rttr::variant variant = create.invoke({}, childTypeName);
					if (variant.is_type<Nightbird::SceneObject*>())
					{
						SceneObject* rawChild = variant.get_value<Nightbird::SceneObject*>();
						rawChild->Deserialize(childJson);
						
						std::unique_ptr<Nightbird::SceneObject> child(rawChild);
						AddChild(std::move(child));
					}
				}
				else
					std::cerr << "SceneObject Deserialize Error: No factory method found for type: " << childTypeName << std::endl;
			}
		}
	}

	void SceneObject::SerializeBase(json& out) const
	{
		rttr::instance instance = *this;
		rttr::type type = instance.get_derived_type();

		out["__type"] = type.get_name().to_string();

		for (auto& property : type.get_properties())
		{
			rttr::variant value = property.get_value(instance);
			if (value.is_valid())
				out["__properties"][property.get_name().to_string()] = SerializeRTTR(value);
		}
	}

	void SceneObject::DeserializeBase(const json& in)
	{
		if (!in.contains("__type"))
		{
			std::cerr << "SceneObject Deserialize Error: Missing __type" << std::endl;
			return;
		}

		std::string typeName = in["__type"];
		rttr::type type = rttr::type::get_by_name(typeName);

		if (!type.is_valid())
		{
			std::cerr << "SceneObject Deserialize Error: Unknown type: " << typeName << std::endl;
			return;
		}

		rttr::instance instance = *this;

		if (in.contains("__properties"))
		{
			const auto& propsJson = in.at("__properties");
			DeserializeRTTR(propsJson, instance);
		}
	}
}

RTTR_REGISTRATION
{
	rttr::registration::class_<Nightbird::SceneObject>("SceneObject")
	.constructor<std::string>()
	.property("name", &Nightbird::SceneObject::name)
	.property("transform", &Nightbird::SceneObject::transform);

	rttr::registration::method("CreateSceneObject", [](const std::string& name) -> Nightbird::SceneObject*
	{
		return new Nightbird::SceneObject(name);
	});
}