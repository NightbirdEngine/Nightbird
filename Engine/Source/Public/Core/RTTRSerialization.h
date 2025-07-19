#pragma once

#include <rttr/type>
#include <nlohmann/json.hpp>

namespace rttr
{
	nlohmann::json SerializeRTTR(const rttr::variant& instance);
	void DeserializeRTTR(const nlohmann::json& json, rttr::instance instance);
}