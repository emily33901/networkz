require("tools/build/premake-modules/export-compile-commands")
require("tools/build/premake-modules/cmake")

local function get_vcpkg_root()
    local handle = io.popen([[for /f "tokens=*" %a in ('where vcpkg.exe') do echo %~dpa]])
    local result = handle:read("*a")
    handle:close()

    local result2 = string.gsub(result, "([^\n]+)\n(.*)\n", "%2"):sub(2)

    return result2
end

if os.host() == "windows" then
    vcpkg_root = (get_vcpkg_root())
    print("vcpkg root is \"" .. vcpkg_root .. "\"")
else
    vcpkg_root = ""
end

local function vcpkg_common()
    filter {"system:windows", "platforms:x64"}
        includedirs{vcpkg_root .. "installed\\x64-windows\\include"}
    filter {"system:windows", "platforms:x32"}
        includedirs{vcpkg_root .. "installed\\x86-windows\\include"}
    filter {}

    filter {"system:windows", "platforms:x64", "configurations:Debug"}
        libdirs {vcpkg_root .. "installed\\x64-windows\\debug\\lib"}
    filter {"system:windows", "platforms:x32", "configurations:Debug"}
        libdirs {vcpkg_root .. "installed\\x86-windows\\debug\\lib"}
    filter {}

    filter {"system:windows", "platforms:x64", "configurations:Release"}
        libdirs {vcpkg_root .. "installed\\x64-windows\\lib"}
    filter {"system:windows", "platforms:x32", "configurations:Debug"}
        libdirs {vcpkg_root .. "installed\\x86-windows\\lib"}
    filter {}
end

local function copy_compile_commands()
    -- For moving the compile commands into the root directory of the project
    -- so that autocomplete tools can see them (cquery...)
    
    -- This is messy but was the only way to get it to work consistently
    -- across multiple platforms (circleci, windows 10, vsts...)
    filter "system:linux"
        prebuildcommands  {
            "{MKDIR} %{wks.location}/compile_commands/",
            "{TOUCH} %{wks.location}/compile_commands/%{cfg.shortname}.json",
            "{COPY} %{wks.location}/compile_commands/%{cfg.shortname}.json ../compile_commands.json"
        }
    filter "system:windows"
        prebuildcommands  {
            "cmd.exe /c \"" .. "{MKDIR} %{wks.location}/compile_commands/",
            "cmd.exe /c \""  .. "{TOUCH} %{wks.location}/compile_commands/%{cfg.shortname}.json",
            "cmd.exe /c \""  .. "{COPY} %{wks.location}/compile_commands/%{cfg.shortname}.json ../compile_commands.json*"
        }
end

local function protogen()

end

workspace "workspace"
    configurations { "Debug", "Release" }
    platforms { "x64", "x32" }
    targetdir "bin/%{cfg.platform}_%{cfg.buildcfg}/%{prj.name}"

    location "premake"
    
    -- Set up platforms 
    filter {"platforms:x32"}
        architecture "x32"
        defines {"ARGONX_32"}
    filter {"platforms:x64"}
        architecture "x64"
        defines {"ARGONX_64"}
    filter {}

    cppdialect "C++17"

    -- Setup c++ spec per platform
    -- Linux uses a buildoption to allow for more
    -- up to date standards (2a)
    filter {"system:windows"}
        toolset "msc-v142"
        characterset "MBCS"

        flags {"MultiProcessorCompile"}

        buildoptions {"-experimental:external -external:W0 -external:anglebrackets -external:templates-"}

        defines {"NETWORKZ_WIN"}

    filter {"system:linux"}
        toolset "clang" -- prefer clang over gcc
        defines {"NETWORKZ_UNIX"}
    filter {}

    -- Setup configurations
    filter "configurations:Debug"
        defines { "DEBUG", "_DEBUG" }
        optimize "Off"
        runtime "Debug"
        
        filter {"system:windows"}
            symbols "Full" -- We need symbols Full instead of just On
        filter {"system:linux"}
            symbols "On"
            buildoptions "-g3 -fdiagnostics-absolute-paths" -- need this for gdb
        filter {}

    filter {"configurations:Release"}
        defines { "NDEBUG" }
        optimize "Full"
        symbols "Full"
        flags {"LinkTimeOptimization"}
        
    filter {}


    project "networkz"
        kind "ConsoleApp"
        language "C++"

        -- Windows and linux use different precompiled header path locations
        filter {"system:linux"}
            pchheader "src/common/precompiled.hh"
        filter {"system:windows"}
            pchheader "src/common/precompiled.hh"
        filter {}
        pchsource "src/common/precompiled.cc"

        files {"src/**.cc", "src/**.hh"}
        includedirs{"src"}

        vcpkg_common()

        copy_compile_commands()