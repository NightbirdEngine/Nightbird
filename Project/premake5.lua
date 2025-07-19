project "Project"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

	defines { "PROJECT_BUILD" }

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
	filter {}
