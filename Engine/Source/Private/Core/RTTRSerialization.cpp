#include <Core/RTTRSerialization.h>

#include <rttr/variant.h>

#include <iostream>

#include <Core/Transform.h>

namespace rttr
{
	nlohmann::json SerializeRTTR(const rttr::variant& variant)
	{
		if (!variant.is_valid())
			return nullptr;
		
		rttr::type type = variant.get_type();

		if (type.is_arithmetic())
		{
			if (type == rttr::type::get<bool>())
				return variant.get_value<bool>();
			if (type == rttr::type::get<int>())
				return variant.get_value<int>();
			if (type == rttr::type::get<float>())
				return variant.get_value<float>();
			if (type == rttr::type::get<double>())
				return variant.get_value<double>();
		}
		
		if (type == rttr::type::get<std::string>())
			return variant.get_value<std::string>();
		
		if (type.is_class())
		{
			nlohmann::json json;
			rttr::instance instance = variant;
			for (const auto& prop : type.get_properties())
			{
				if (!prop.is_valid())
					continue;

				rttr::variant propValue = prop.get_value(instance);
				if (!propValue.is_valid())
					continue;
				
				json[prop.get_name().to_string()] = SerializeRTTR(propValue);
			}
			return json;
		}

		return variant.to_string();
	}

	void DeserializeRTTR(const nlohmann::json& json, rttr::instance instance)
	{
		rttr::type type = instance.get_derived_type();

		if (!type.is_class())
			return;

		for (const auto& prop : type.get_properties())
		{
			std::string propName = prop.get_name().to_string();

			if (!prop.is_valid() || prop.is_readonly())
				continue;

			if (!json.contains(propName))
				continue;

			rttr::type propType = prop.get_type();
			const nlohmann::json& propJson = json.at(propName);

			rttr::variant variant;

			if (propType.is_arithmetic())
			{
				if (propType == rttr::type::get<bool>())
					variant = propJson.get<bool>();
				if (propType == rttr::type::get<int>())
					variant = propJson.get<int>();
				if (propType == rttr::type::get<float>())
					variant = propJson.get<float>();
				if (propType == rttr::type::get<double>())
					variant = propJson.get<double>();
			}
			else if (propType == rttr::type::get<std::string>())
				variant = propJson.get<std::string>();
			else if (propType.is_class())
			{
				rttr::variant propVariant = prop.get_value(instance);
				if (propVariant.is_valid())
				{
					rttr::instance propInstance = propVariant;
					if (!propInstance.is_valid())
					{
						std::cerr << "Failed to convert variant of property " << propName << " to instance" << std::endl;
						continue;
					}
					DeserializeRTTR(propJson, propInstance);
					variant = propVariant;
				}
			}

			if (variant.is_valid())
			{
				bool success = prop.set_value(instance, variant);
				if (!success)
					std::cerr << "Failed to set value for property: " << propName << std::endl;
			}
		}
	}
}