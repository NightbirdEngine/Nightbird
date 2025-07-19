dofile("../rttr_copy.lua")

project "App"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	local outBinDir = "%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}"

	targetdir (outBinDir)
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

	debugdir (outBinDir)

	defines { "VK_NO_PROTOTYPES" }
	defines { "GLFW_INCLUDE_VULKAN" }
	defines { "IMGUI_IMPL_VULKAN_USE_VOLK" }

	files {
		"Source/**.h",
		"Source/**.cpp"
	}

	includedirs {
		"Source/Public",
		"%{wks.location}/Engine/Source/Public",
		"%{wks.location}/Engine/Vendor/vulkan-headers/include",
		"%{wks.location}/Engine/Vendor/volk",
		"%{wks.location}/Engine/Vendor/vma",
		"%{wks.location}/Engine/Vendor/glfw/include",
		"%{wks.location}/Engine/Vendor/glm",
		"%{wks.location}/Engine/Vendor/stb",
		"%{wks.location}/Engine/Vendor/fastgltf/include",
		"%{wks.location}/Engine/Vendor/rttr/src",
		"%{wks.location}/Engine/Vendor/json"
	}

	links { "Engine" }

	filter { "system:windows" }
		links { "rttr" }
		copy_rttr("windows", outputdir)
	filter { "system:linux" }
		copy_rttr("linux", outputdir)
	filter {}
