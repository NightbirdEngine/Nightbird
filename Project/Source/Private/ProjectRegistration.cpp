#include <Core/ProjectRegistration.h>

//#include <rttr/registration.h>

#include <CustomObject.h>

#include <iostream>

void RegisterTypes()
{
	//rttr::registration::class_<CustomObject>("CustomObject")
	//	.constructor<std::string>()
	//	.property("testVar", &CustomObject::testVar);

	//rttr::registration::method("CreateCustomObject", [](const std::string& name) -> Nightbird::SceneObject*
	//	{
	//		return new CustomObject(name);
	//	});
	//
	//for (auto& type : rttr::type::get_types())
	//{
	//	std::cout << "PROJECT: " << type.get_name() << std::endl;
	//}
}