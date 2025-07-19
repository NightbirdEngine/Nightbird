project "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

	defines { "VK_NO_PROTOTYPES" }
	defines { "VMA_DYNAMIC_VULKAN_FUNCTIONS" }
	defines { "GLFW_INCLUDE_VULKAN" }

	files {
		"Source/**.h",
		"Source/**.cpp"
	}

	includedirs {
		"Source/Public",
		"Vendor/vulkan-headers/include",
		"Vendor/volk",
		"Vendor/vma",
		"Vendor/glfw/include",
		"Vendor/glm",
		"Vendor/stb",
		"Vendor/fastgltf/include",
		"Vendor/rttr/src",
		"Vendor/json"
	}

	links { "glfw", "fastgltf" }

	filter { "system:windows" }
		links { "rttr" }
	filter {}
