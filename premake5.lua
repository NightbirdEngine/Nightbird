workspace "Nightbird"
	architecture "x86_64"
	startproject "Editor"

	configurations { "Debug", "Release" }

	filter { "configurations:Debug"}
		defines { "DEBUG" }
		symbols "On"
		optimize "Off"
		runtime "Debug"

	filter { "configurations:Release" }
		defines { "NDEBUG" }
		optimize "On"
		runtime "Release"

	filter { }

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "Engine/Vendor/glfw"
	include "Engine/Vendor/fastgltf"
	include "Engine/Vendor/rttr"
	include "Editor/vendor/imgui"
group ""

group "Nightbird"
	include "Engine"
	include "App"
	include "Editor"
group ""

group "Projects"
	include "Project"
group ""
