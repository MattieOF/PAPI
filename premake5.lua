workspace "PAPI"
	configurations { "Debug", "Release", "Dist" }
	platforms { "Win64" }
	startproject "PAPI"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["spdlog"] = "PAPI/Vendor/spdlog/include"
IncludeDir["SDL"] = "PAPI/Vendor/SDL/include"
IncludeDir["imgui"] = "PAPI/Vendor/imgui/"

--include "PAPI/Vendor/imgui.lua"

project "PAPI"
	kind "ConsoleApp"
	staticruntime "On"
	language "C++"
	location "PAPI"
	targetdir ("Build/%{prj.name}/" .. outputdir)
	objdir ("Build/%{prj.name}/Intermediates/" .. outputdir)

	pchheader "papipch.h"
	pchsource "PAPI/Source/papipch.cpp"

	vpaths {
		["Include"] = {"PAPI/Include/**.h", "PAPI/Include/**.hpp"},
		["Source"] = {"PAPI/Source/**.cpp", "PAPI/Source/**.c"},
	}

	files { 
		"PAPI/Include/**.h", "PAPI/Include/**.hpp", 
		"PAPI/Source/**.cpp", "PAPI/Source/**.c",

		"PAPI/Vendor/PPK_ASSERT/Source/*.cpp"
	}

	includedirs 
	{ 
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.SDL}",
		"%{IncludeDir.imgui}",

		"PAPI/Include"
	}

	libdirs 
	{
		"PAPI/Vendor/SDL/lib"
	}

	links
	{
		"imgui",
		"SDL3-static"
	}

	defines {
		"_CRT_SECURE_NO_WARNINGS"
	}

os.mkdir("PAPI/Source")
os.mkdir("PAPI/Include")

filter "configurations:Debug"
	defines { "PAPI_DEBUG", "PAPI_ENABLE_ASSERTS" }
	symbols "On"
	runtime "Debug"

filter "configurations:Release"
	defines { "PAPI_RELEASE", "PAPI_ENABLE_ASSERTS" }
	optimize "On"
	symbols "On"
	runtime "Release"

filter "configurations:Dist"
	defines { "PAPI_DIST" }
	kind "WindowedApp"
	optimize "On"
	symbols "Off"
	runtime "Release"

filter "system:windows"
	cppdialect "C++17"
	systemversion "latest"
	defines { "PAPI_PLATFORM_WINDOWS" }

	links
	{
		"version",
		"winmm",
		"Imm32",
		"Cfgmgr32",
		"Setupapi"
	}

filter "platforms:Win64"
	system "Windows"
	architecture "x64"