#include <Inspector.h>

#include <ImGuiOverlay.h>

#include <Core/SceneObject.h>
#include <Core/Scene.h>
#include <Core/Camera.h>
#include <Core/PointLight.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

inline float Wrap180(float angle)
{
	angle = std::fmod(angle + 180.0f, 360.0f);
	if (angle < 0.0f)
		angle += 360.0f;
	return angle - 180.0f;
}

inline glm::vec3 WrapEuler180(const glm::vec3 angles)
{
	return glm::vec3(Wrap180(angles.x), Wrap180(angles.y), Wrap180(angles.z));
}

inline float RoundDP(float value, int dp)
{
	float factor = (float)std::pow(10.0f, dp);
	return std::round(value * factor) / factor;
}

inline glm::vec3 RoundEulerDP(const glm::vec3& angles, int dp)
{
	return glm::vec3(RoundDP(angles.x, dp), RoundDP(angles.y, dp), RoundDP(angles.z, dp));
}

namespace Nightbird
{
	Inspector::Inspector(Scene* scene, VulkanImGuiOverlay* overlay, bool open)
		: ImGuiWindow("Inspector", open), m_Scene(scene), m_Overlay(overlay)
	{

	}

	void Inspector::OnRender()
	{
		SceneObject* selectedObject = m_Overlay->GetSelectedObject();
		if (selectedObject)
		{
			RenderProperties(rttr::instance(*selectedObject));
			
			//const float xPos = 80.0f;

			//if (ImGui::CollapsingHeader("Transform"))
			//{
			//	ImGui::Text("Position");
			//	ImGui::SameLine();
			//	ImGui::SetCursorPosX(xPos);
			//	ImGui::DragFloat3("##Position", &selectedObject->transform.position[0], 0.01f, 0.0f, 0.0f, "%g");

			//	static void* lastObject = nullptr;
			//	if (selectedObject != lastObject)
			//	{
			//		cachedEulerDegrees = glm::degrees(glm::eulerAngles(selectedObject->transform.rotation));
			//		lastObject = selectedObject;
			//	}

			//	ImGui::Text("Rotation");
			//	ImGui::SameLine();
			//	ImGui::SetCursorPosX(xPos);
			//	if (ImGui::DragFloat3("##Rotation", &cachedEulerDegrees[0], 0.1f, 0.0f, 0.0f, "%g"))
			//	{
			//		cachedEulerDegrees = WrapEuler180(cachedEulerDegrees);
			//		cachedEulerDegrees = RoundEulerDP(cachedEulerDegrees, 2);
			//		glm::vec3 eulerRadians = glm::radians(cachedEulerDegrees);

			//		glm::quat yaw = glm::angleAxis(eulerRadians.y, glm::vec3(0.0f, 1.0f, 0.0f));
			//		glm::quat pitch = glm::angleAxis(eulerRadians.x, glm::vec3(1.0f, 0.0f, 0.0f));
			//		glm::quat roll = glm::angleAxis(eulerRadians.z, glm::vec3(0.0f, 0.0f, 1.0f));

			//		// Translate back to radians and quaternion for internal memory
			//		selectedObject->transform.rotation = yaw * pitch * roll;
			//	}

			//	ImGui::Text("Scale");
			//	ImGui::SameLine();
			//	ImGui::SetCursorPosX(xPos);
			//	ImGui::DragFloat3("##Scale", &selectedObject->transform.scale[0], 0.01f, 0.0f, 0.0f, "%g");
			//}

			//if (Camera* camera = dynamic_cast<Camera*>(selectedObject))
			//{
			//	if (ImGui::CollapsingHeader("Camera"))
			//	{
			//		ImGui::Text("FOV");
			//		ImGui::SameLine();
			//		ImGui::SetCursorPosX(xPos);
			//		ImGui::DragFloat("##FOV", &camera->fov, 0.1f, 1.0f, 179.0f, "%g");
			//	}
			//}
			//else if (PointLight* pointLight = dynamic_cast<PointLight*>(selectedObject))
			//{
			//	if (ImGui::CollapsingHeader("Point Light"))
			//	{
			//		ImGui::Text("Intensity");
			//		ImGui::SameLine();
			//		ImGui::SetCursorPosX(xPos);
			//		ImGui::DragFloat("##Intensity", &pointLight->intensity, 0.01f, 0.1f, 100.0f, "%g");

			//		ImGui::Text("Color");
			//		ImGui::SameLine();
			//		ImGui::SetCursorPosX(xPos);
			//		ImGui::ColorPicker3("##Color", &pointLight->color.r);

			//		ImGui::Text("Radius");
			//		ImGui::SameLine();
			//		ImGui::SetCursorPosX(xPos);
			//		ImGui::DragFloat("##Radius", &pointLight->radius, 0.01f, 0.01f, 100.0f, "%g");
			//	}
			//}
		}
	}

	void Inspector::RenderProperties(rttr::instance instance)
	{
		rttr::type type = instance.get_derived_type();

		for (auto& property : type.get_properties())
		{
			if (!property.is_valid() || property.is_readonly())
				continue;

			rttr::variant value = property.get_value(instance);
			rttr::type valueType = value.get_type();
			std::string label = property.get_name().to_string();

			ImGui::PushID(label.c_str());

			if (valueType == rttr::type::get<int>())
			{
				int intValue = value.get_value<int>();
				if (ImGui::DragInt(label.c_str(), &intValue))
					property.set_value(instance, intValue);
			}
			else if (valueType == rttr::type::get<float>())
			{
				float floatValue = value.get_value<float>();
				if (ImGui::DragFloat(label.c_str(), &floatValue))
					property.set_value(instance, floatValue);
			}
			else if (valueType == rttr::type::get<bool>())
			{
				bool boolValue = value.get_value<bool>();
				if (ImGui::Checkbox(label.c_str(), &boolValue))
					property.set_value(instance, boolValue);
			}
			else if (valueType == rttr::type::get<std::string>())
			{
				std::string strValue = value.get_value<std::string>();
				char buffer[256];
				strncpy_s(buffer, strValue.c_str(), sizeof(buffer));
				if (ImGui::InputText(label.c_str(), buffer, sizeof(buffer)))
					property.set_value(instance, std::string(buffer));
			}
			else if (valueType == rttr::type::get<glm::vec3>())
			{
				glm::vec3 vec3Value = value.get_value<glm::vec3>();
				if (ImGui::DragFloat3(label.c_str(), &vec3Value[0], 0.01f))
					property.set_value(instance, vec3Value);
			}
			else if (valueType == rttr::type::get<Transform>())
			{
				Transform transform = value.get_value<Transform>();

				ImGui::Text("%s", label.c_str());

				ImGui::PushID((label + "_Transform").c_str());

				if (ImGui::DragFloat3("Position", &transform.position[0], 0.01f))
					property.set_value(instance, transform);
				
				glm::vec3 cachedDegrees = glm::degrees(glm::eulerAngles(transform.rotation));
				if (ImGui::DragFloat3("Rotation", &cachedDegrees[0], 0.01f))
				{
					cachedDegrees = WrapEuler180(cachedDegrees);
					cachedDegrees = RoundEulerDP(cachedDegrees, 2);
					glm::vec3 eulerRadians = glm::radians(cachedDegrees);
					
					glm::quat yaw = glm::angleAxis(eulerRadians.y, glm::vec3(0.0f, 1.0f, 0.0f));
					glm::quat pitch = glm::angleAxis(eulerRadians.x, glm::vec3(1.0f, 0.0f, 0.0f));
					glm::quat roll = glm::angleAxis(eulerRadians.z, glm::vec3(0.0f, 0.0f, 1.0f));
					
					// Translate back to radians and quaternion for internal memory
					transform.rotation = yaw * pitch * roll;

					property.set_value(instance, transform);
				}

				if (ImGui::DragFloat3("Scale", &transform.scale[0], 0.01f))
					property.set_value(instance, transform);

				ImGui::PopID();
			}

			ImGui::PopID();
		}
	}
}