package.path = package.path .. ";../Assets/Scripts/?.lua"

require("common.common")
require("common.global")
require("scene")

Logger.Info("Load main file success.")

MainInstance = ClassDefinition(MainInstance);
function MainInstance:ctor()
	self.m_main_scene = MainScene:new();
end 

function MainInstance:onMainInitialize()
	Logger.Info("onMainInitialize")

	collectgarbage('setpause', 200)
	collectgarbage('setstepmul', 400)

	-- initialize modules
	global_context.m_addon_manager:initialize();

	self.m_main_scene:initialize();
end 

function MainInstance:onMainStart()
	Logger.Info("onMainStart")

	self.m_main_scene:onStart();
end

function MainInstance:onMainUpdate()
	local deltaTime = System.GetDeltaTime();

	self.m_main_scene:update(deltaTime);
end 

function MainInstance:onMainUnInitialize()
	Logger.Info("OnMainUnInitialize")

	self.m_main_scene:uninitialize();

	-- uninitiaize modules
	global_context.m_addon_manager:uninitialize();
end 

mainInstance = MainInstance:new();
function clinet_main_initialize() mainInstance:onMainInitialize(); end 
function clinet_main_start() mainInstance:onMainStart(); end 
function client_main_tick() mainInstance:onMainUpdate(); end 
function client_main_stop() mainInstance:onMainUnInitialize(); end 

SystemExports[SystemFunctionIndex.CLIENT_LUA_MAIN_INITIALIZE] = clinet_main_initialize;
SystemExports[SystemFunctionIndex.CLIENT_LUA_MAIN_START] = clinet_main_start;
SystemExports[SystemFunctionIndex.CLIENT_LUA_MAIN_UPDATE] = client_main_tick;
SystemExports[SystemFunctionIndex.CLIENT_LUA_MAIN_STOP] = client_main_stop;