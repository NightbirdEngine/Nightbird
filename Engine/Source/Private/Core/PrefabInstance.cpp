#include <Core/PrefabInstance.h>

namespace Nightbird
{
	PrefabInstance::PrefabInstance(const char* name, const char* prefabPath)
		: SceneObject(name), prefabPath(prefabPath)
	{

	}

	PrefabInstance::PrefabInstance(const std::string& name, const std::string& prefabPath)
		: SceneObject(name), prefabPath(prefabPath)
	{

	}

	PrefabInstance::~PrefabInstance()
	{

	}

	const std::string& PrefabInstance::GetPrefabPath() const
	{
		return prefabPath;
	}

	void PrefabInstance::Serialize(json& out) const
	{
		SerializeBase(out);
	}

	void PrefabInstance::Deserialize(const json& in)
	{
		DeserializeBase(in);
	}
}

RTTR_REGISTRATION
{
	rttr::registration::class_<Nightbird::PrefabInstance>("PrefabInstance")
	.constructor<std::string>()
	.property("PrefabPath", &Nightbird::PrefabInstance::prefabPath);

	rttr::registration::method("CreatePrefabInstance", [](const std::string& name) -> Nightbird::SceneObject*
	{
		return new Nightbird::PrefabInstance(name);
	});
}