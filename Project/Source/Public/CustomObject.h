#pragma once

#include <string>

#include <Core/SceneObject.h>

class CustomObject : public Nightbird::SceneObject
{
public:
	CustomObject(const std::string& name);
	
	int testVar = 5;

	RTTR_ENABLE(Nightbird::SceneObject)
};