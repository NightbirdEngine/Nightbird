#include <CreateObjectWindow.h>

#include <array>
#include <iostream>

#include <Core/Scene.h>
#include <Core/Transform.h>
#include <Core/SceneObject.h>

namespace Nightbird
{
	CreateObjectWindow::CreateObjectWindow(Scene* scene, bool open)
		: ImGuiWindow("Create Object", open, ImGuiWindowProperties{false, true, ImVec2(400, 600)}), m_Scene(scene)
	{

	}
	
	void CreateObjectWindow::OnRender()
	{
		static std::vector<std::string> objectTypeNames;
		static std::vector<rttr::type> objectTypes;
		static bool objectTypesDirty = true;

		if (objectTypesDirty)
		{
			objectTypeNames.clear();
			objectTypes.clear();

			for (auto& type : rttr::type::get_types())
			{
				if (type.is_derived_from(rttr::type::get<Nightbird::SceneObject>()))
				{
					auto constructor = type.get_constructor({rttr::type::get<std::string>()});
					if (constructor.is_valid())
					{
						objectTypeNames.push_back(type.get_name().to_string());
						objectTypes.push_back(type);
					}
				}
			}
			
			objectTypesDirty = false;
		}

		static size_t selected = static_cast<size_t>(-1);
		
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 15.0f));

		ImGui::Text("Create an empty scene object.");

		ImGui::PopStyleVar(); // Pop spacing

		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.0f, 0.5f));
		for (int i = 0; i < objectTypeNames.size(); ++i)
		{
			if (ImGui::Selectable(objectTypeNames[i].c_str(), selected == i, 0, ImVec2(0.0f, 30.0f)))
				selected = i;
		}
		ImGui::PopStyleVar(); // Pop selectable text align

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 15.0f));

		ImGui::Dummy(ImVec2(0.0f, 0.0f));

		ImGui::BeginDisabled(selected == -1);
		if (ImGui::Button("Create"))
		{
			if (selected >= 0 && selected < objectTypes.size())
			{
				rttr::type selectedType = objectTypes[selected];
				auto create = rttr::type::get_global_method("Create" + selectedType.get_name().to_string());
				if (create.is_valid())
				{
					rttr::variant variant = create.invoke({}, selectedType.get_name().to_string());
					if (variant.is_type<Nightbird::SceneObject*>())
					{
						SceneObject* rawObject = variant.get_value<Nightbird::SceneObject*>();
						std::unique_ptr<Nightbird::SceneObject> object(rawObject);
						m_Scene->AddSceneObject(std::move(object));
					}
				}
			}
			
			m_Open = false;
			selected = static_cast<size_t>(-1);
		}
		ImGui::EndDisabled();

		ImGui::PopStyleVar();
	}
}