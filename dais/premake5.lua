project "dais"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
    }

    includedirs
    {
        "src",
    }

    files
    {
        "src/main.cpp",
        "src/engine/**.h",
        "src/engine/**.c",
        "src/engine/**.hpp",
        "src/engine/**.cpp",

        "src/external/**.h",
        "src/external/**.c",
        "src/external/**.hpp",
        "src/external/**.cpp",
    }

    filter { "system:Windows" }
        systemversion "latest"
		files
		{
			"src/platform/windows/**.h",
			"src/platform/windows/**.c",
			"src/platform/windows/**.hpp",
			"src/platform/windows/**.cpp",
		}
		links
		{
			"opengl32.lib"
		}

    filter { "system:Linux" }
		files
		{
			"src/platform/linux/**.h",
			"src/platform/linux/**.c",
			"src/platform/linux/**.hpp",
			"src/platform/linux/**.cpp",
		}

    filter { "configurations:Debug" }
        defines "DAIS_DEBUG"
        runtime "Debug"
        symbols "on"

    filter { "configurations:Release" }
        defines "DAIS_RELEASE"
        runtime "Release"
        optimize "on"

    filter { "configurations:Dist" }
        defines "DAIS_DIST"
        runtime "Release"
        optimize "on"

