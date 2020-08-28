require('vstudio')

-----------------------------------------------------------------------------------
-- DEFINITIONS
-----------------------------------------------------------------------------------
SOLUTION_NAME       = "Cjing3D"
CJING3D_NAME        = "Cjing3D"
LUA_GENERATER_NAME  = "LuaGenerator"
TARGET_NAME         = "Cjing3D"
SHADER_NAME         = "Shader"
GAME_NAME           = "Game"
EDITOR_NAME         = "Editor"
ASSETS_NAME         = "Assets"

LIBRARY_DIR	        = "../lib"
DEBUG_FORMAT        = "c7"
TARGET_OBJ_DIR      = "/Temp"
TARGET_DIR_DEBUG    = "../bin/x64/Debug"
TARGET_DIR_RELEASE  = "../bin/x64/Release"
IGNORE_FILES		= {}

CJING3D_DIR         = "../" .. CJING3D_NAME
LUA_GENERATER_DIR   = "../" .. LUA_GENERATER_NAME
SHADER_DIR          = "../" .. SHADER_NAME
ASSETS_DIR          = "../" .. ASSETS_NAME
GAME_DIR            = "../" .. GAME_NAME
EDITOR_DIR          = "../" .. EDITOR_NAME

local function IncludeThirdParty()
    includedirs { "../ThirdParty/imgui" }
    includedirs { "../ThirdParty/optick" }
    includedirs { "../ThirdParty/physfs_3.0.2" }
end 

-----------------------------------------------------------------------------------
-- Custom
-----------------------------------------------------------------------------------
premake.api.register {
    name = "conformanceMode",
    scope = "config",
    kind = "boolean",
    default = true
}

local function HandleConformanceMode(cfg)
    if cfg.conformanceMode ~= nil and cfg.conformanceMode == true then
        premake.w('<ConformanceMode>true</ConformanceMode>')
    end
end
  
premake.override(premake.vstudio.vc2010.elements, "clCompile", function(base, cfg)
    local calls = base(cfg)
    table.insert(calls, HandleConformanceMode)
    return calls
end)

-----------------------------------------------------------------------------------
-- ignore files
-----------------------------------------------------------------------------------
IGNORE_FILES[0] = CJING3D_DIR .. "/resource/modelImporter/modelImporterAssimp.cpp"

-----------------------------------------------------------------------------------
-- SOLUTION
-----------------------------------------------------------------------------------
solution (SOLUTION_NAME)
    location ".."
    systemversion "latest"
    cppdialect "C++17"
    language "C++"
    platforms "x64"
    configurations {"Debug","Release"}

    filter { "platforms:x64" }
        system "Windows"
        architecture "x64"

    -- Debug config
    filter {"configurations:Debug"}
        defines { "DEBUG" }
        flags { "MultiProcessorCompile"}
        symbols "On"

    -- Release config
    filter {"configurations:Release"}
        defines { "NDEBUG" }
        flags { "MultiProcessorCompile"}
        optimize "On"

    -- Reset the filter for other settings
	filter { }

-----------------------------------------------------------------------------------
-- Game
-----------------------------------------------------------------------------------
project (GAME_NAME)
    location(GAME_DIR)
    objdir(GAME_DIR .. TARGET_OBJ_DIR)
    dependson { CJING3D_NAME, SHADER_NAME, LUA_GENERATER_NAME }
    targetname(GAME_NAME)
    links { CJING3D_NAME }
    kind "WindowedApp"
    staticruntime "Off"
    conformanceMode(true)
    
    -- Files
	files 
	{ 
        GAME_DIR .. "/**.c",
		GAME_DIR .. "/**.cpp",
        GAME_DIR .. "/**.hpp",
        GAME_DIR .. "/**.h",
		GAME_DIR .. "/**.inl"
    }
    
    -- Includes
    includedirs { CJING3D_DIR }
    includedirs { GAME_DIR }
    includedirs { SHADER_DIR }

    IncludeThirdParty();

    -- Libraries
    libdirs (LIBRARY_DIR)

    -- Debug config
    filter {"configurations:Debug"}
        targetdir (TARGET_DIR_DEBUG)
        debugdir (TARGET_DIR_DEBUG)
        debugformat (DEBUG_FORMAT)
        links {"OptickCoreD"}


    -- Release config
    filter {"configurations:Release"}
        targetdir (TARGET_DIR_RELEASE)
        debugdir (TARGET_DIR_RELEASE)
        links {"OptickCore.lib"}

-----------------------------------------------------------------------------------
-- Editor
-----------------------------------------------------------------------------------
project (EDITOR_NAME)
    location(EDITOR_DIR)
    objdir(EDITOR_DIR .. TARGET_OBJ_DIR)
    dependson { CJING3D_NAME, SHADER_NAME, LUA_GENERATER_NAME }
    targetname(EDITOR_NAME)
    links { CJING3D_NAME }
    kind "WindowedApp"
    staticruntime "Off"
    conformanceMode(true)

    -- Files
    files 
    { 
        EDITOR_DIR .. "/**.c",
        EDITOR_DIR .. "/**.cpp",
        EDITOR_DIR .. "/**.hpp",
        EDITOR_DIR .. "/**.h",
        EDITOR_DIR .. "/**.inl",
        "../ThirdParty/imgui/**.h",
        "../ThirdParty/imgui/**.cpp"
    }

    -- Includes
    includedirs { CJING3D_DIR }
    includedirs { EDITOR_DIR }
    includedirs { SHADER_DIR }

    IncludeThirdParty();

    vpaths { ["editor/imgui/src"] =  {"../ThirdParty/imgui/**.h", "../ThirdParty/imgui/**.cpp"}}

    -- Libraries
    libdirs (LIBRARY_DIR)

    -- Debug config
    filter {"configurations:Debug"}
        targetdir (TARGET_DIR_DEBUG)
        debugdir (TARGET_DIR_DEBUG)
        debugformat (DEBUG_FORMAT)
        links {"OptickCoreD"}


    -- Release config
    filter {"configurations:Release"}
        targetdir (TARGET_DIR_RELEASE)
        debugdir (TARGET_DIR_RELEASE)
        links {"OptickCore.lib"}       

-----------------------------------------------------------------------------------
-- Cjing3D
-----------------------------------------------------------------------------------
project (CJING3D_NAME)
    location(CJING3D_DIR)
    objdir(CJING3D_DIR .. TARGET_OBJ_DIR)
    dependson { SHADER_NAME, LUA_GENERATER_NAME }
    kind "StaticLib"
    staticruntime "Off"
    conformanceMode(true)
    
    -- Files
	files 
	{ 
        CJING3D_DIR .. "/**.c",
		CJING3D_DIR .. "/**.cpp",
        CJING3D_DIR .. "/**.hpp",
        CJING3D_DIR .. "/**.h",
        CJING3D_DIR .. "/**.inl",
        CJING3D_DIR .. "/**..bat",
    }

    -- ignore
	removefiles { IGNORE_FILES[0] }
    
    -- Includes
    includedirs { CJING3D_DIR }
    includedirs { SHADER_DIR }

    IncludeThirdParty();

    -- Libraries
    libdirs (LIBRARY_DIR)
    
    -- prebuild
    prebuildcommands {
        "$(SolutionDir)Cjing3D/precompile.bat $(Platform) $(Configuration)"
    }

    -- Debug config
    filter {"configurations:Debug"}
        targetdir (TARGET_DIR_DEBUG)
        debugdir (TARGET_DIR_DEBUG)
        debugformat (DEBUG_FORMAT)
        links {"physfs-static-debug.lib"}
        links {"dinput8"}

    -- Release config
    filter {"configurations:Release"}
        targetdir (TARGET_DIR_RELEASE)
        debugdir (TARGET_DIR_RELEASE)
        links {"physfs-static.lib"}
        links {"dinput8"}

-----------------------------------------------------------------------------------
-- LuaGenerate
-----------------------------------------------------------------------------------
project (LUA_GENERATER_NAME)
    location(LUA_GENERATER_DIR)
    objdir(LUA_GENERATER_DIR .. TARGET_OBJ_DIR)
    targetname(LUA_GENERATER_NAME)
    kind "ConsoleApp"
    staticruntime "Off"
    
    -- Files
	files 
	{ 
		LUA_GENERATER_DIR .. "/**.cpp",
        LUA_GENERATER_DIR .. "/**.hpp",
        LUA_GENERATER_DIR .. "/**.h",
		LUA_GENERATER_DIR .. "/**.inl"
    }
    
    -- ignore
	removefiles { IGNORE_FILES[0] }
    
    -- Includes
    includedirs { LUA_GENERATER_DIR }
    includedirs { "../ThirdParty/physfs_3.0.2" }

    -- Libraries
    libdirs (LIBRARY_DIR)

    -- Debug config
    filter {"configurations:Debug"}
        targetdir (TARGET_DIR_DEBUG)
        debugdir (TARGET_DIR_DEBUG)
        debugformat (DEBUG_FORMAT)
        links {"physfs-static-debug.lib"}

    -- Release config
    filter {"configurations:Release"}
        targetdir (TARGET_DIR_RELEASE)
        debugdir (TARGET_DIR_RELEASE)
        links {"physfs-static.lib"}

-----------------------------------------------------------------------------------
-- Shader
-----------------------------------------------------------------------------------
project (SHADER_NAME)
    location(SHADER_DIR)
    objdir(SHADER_DIR .. TARGET_OBJ_DIR)
	kind "WindowedApp"
    staticruntime "On"
    targetdir (TARGET_DIR_DEBUG)
    debugdir (TARGET_DIR_DEBUG)
    shadermodel("5.0")

    -- Files
	files 
	{ 
		SHADER_DIR .. "/**.hlsl",
        SHADER_DIR .. "/**.hlsli",
        SHADER_DIR .. "/**.h",
        SHADER_DIR .. "/**..bat",
    }
    
    -- ignore
	removefiles { IGNORE_FILES[0] }
    
    -- post
    postbuildcommands {
        "$(SolutionDir)Shader/postcompile.bat $(Platform) $(Configuration) $(OutDir)"
    }

    -- shader files config
    filter("files:**.hlsl")
        shaderobjectfileoutput(ASSETS_DIR.."/Shaders/%{file.basename}"..".cso")
    filter {"files:" .. SHADER_DIR .. "/vs/**.hlsl"}
        shadertype("Vertex")
    filter {"files:" .. SHADER_DIR .. "/ps/**.hlsl"}
        shadertype("Pixel")
    filter {"files:" .. SHADER_DIR .. "/cs/**.hlsl"}
        shadertype("Compute")
    filter {"files:" .. SHADER_DIR .. "/ds/**.hlsl"}
        shadertype("Domain")
    filter {"files:" .. SHADER_DIR .. "/hs/**.hlsl"}
        shadertype("Hull")