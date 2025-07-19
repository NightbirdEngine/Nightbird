project "fastgltf"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

	files
	{
		"**.h",
		"**.hpp",
		"**.cpp"
	}

	includedirs
	{
		"include"
	}
