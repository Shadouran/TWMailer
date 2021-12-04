workspace "TWMailer"
    architecture "x64"
    startproject "server"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "twmailer-server"
    location "twmailer-server"
    kind "ConsoleApp"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    warnings "high"
    flags
    {
        "fatalwarnings"
    }

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
    }

    filter "system:windows" --until next filter or project
        cppdialect "C++20"
        staticruntime "On"
        systemversion "latest"

        defines
        {
        }

    filter "configurations:Debug"
        symbols "On"
 
    filter "configurations:Release"
        optimize "On"

    filter "configurations:Dist"
        optimize "On"

        
project "twmailer-client"
    location "twmailer-client"
    kind "ConsoleApp"
    language "C++"

    targetdir ( "bin/" .. outputdir .. "/%{prj.name}" )
    objdir ( "bin-int/" .. outputdir .. "/%{prj.name}" )

    warnings "high"
    flags
    {
        "fatalwarnings"
    }

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
    }
    
    filter "system:windows" --until next filter or project
        cppdialect "C++20"
        staticruntime "On"
        systemversion "latest"

        defines
        {
        }

    filter "configurations:Debug"
        symbols "On"
 
    filter "configurations:Release"
        optimize "On"

    filter "configurations:Dist"
        optimize "On"