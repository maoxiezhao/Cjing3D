package.path = package.path .. ";GameAssets/Scripts/?.lua"

require("common.common")
require("common.global")
require("scene.editorScene")

MainInstance = ClassDefinition(MainInstance);
function MainInstance:ctor()
end 

function MainInstance:onMainInitialize()
	Logger.Info("onMainInitialize")

	collectgarbage('setpause', 200)
	collectgarbage('setstepmul', 400)

	-- initialize modules
	global_context.m_addon_manager:initialize();

	-- initialize scene manager
	global_context.m_scene_manager:initialize();
end

function MainInstance:onMainStart()
	Logger.Info("onMainStart")
end

function MainInstance:onMainUpdate()
end 

function MainInstance:onMainFixedUpdate()
	local deltaTime = System.GetDeltaTime();
	global_context.m_scene_manager:update(deltaTime);
end 

function MainInstance:onMainUnInitialize()
	Logger.Info("OnMainUnInitialize")

	-- uninitialzie scene manager
	global_context.m_scene_manager:uninitialize();

	-- uninitialize modules
	global_context.m_addon_manager:uninitialize();
end 

mainInstance = MainInstance:new();
function clinet_main_initialize() mainInstance:onMainInitialize(); end 
function clinet_main_start() mainInstance:onMainStart(); end 
function client_main_tick() mainInstance:onMainUpdate(); end 
function client_main_stop() mainInstance:onMainUnInitialize(); end 
function client_main_fixedUpdate() mainInstance:onMainFixedUpdate(); end
function client_main_change_scene(name)
	local class_def = _G[name];
	if class_def == nil then 
		return;
	end 
	global_context.m_scene_manager:setNextScene(class_def:new());
end 

SystemExports[SystemFunctionIndex.CLIENT_LUA_MAIN_INITIALIZE] = clinet_main_initialize;
SystemExports[SystemFunctionIndex.CLIENT_LUA_MAIN_START] = clinet_main_start;
SystemExports[SystemFunctionIndex.CLIENT_LUA_MAIN_UPDATE] = client_main_tick;
SystemExports[SystemFunctionIndex.CLIENT_LUA_MAIN_FIXED_UPDATE] = client_main_fixedUpdate;
SystemExports[SystemFunctionIndex.CLIENT_LUA_MAIN_STOP] = client_main_stop;
SystemExports[SystemFunctionIndex.CLIENT_LUA_MAIN_CHANGE_SCENE] = client_main_change_scene;