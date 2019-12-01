package.path = package.path .. ";../Assets/Scripts/?.lua"

require("common.common")
require("common.global")
require("camera")

Logger.Info("Load main file success.")

local MainInstance = ClassDefine(MainInstance);
function MainInstance:ctor()
	self.m_fps_camera = FPSCamera:new();
end 

function MainInstance:onMainInitialize()
	Logger.Info("onMainInitialize")

	collectgarbage('setpause', 200)
	collectgarbage('setstepmul', 400)
end 

function MainInstance:onMainStart()
	Logger.Info("onMainStart")

	local main_camera = Render.GetMainCamera();
	self.m_fps_camera:setCamera(main_camera);
	self.m_fps_camera:reset(Vector:new(300, 200, -300));
	self.m_fps_camera:setMoveSpeed(50);
end

function MainInstance:onMainUpdate()
	local deltaTime = System.GetDeltaTime();

	self.m_fps_camera:update(deltaTime);
end 

function MainInstance:onMainUnInitialize()
	Logger.Info("OnMainUnInitialize")
end 

local mainInstance = MainInstance:new();
function clinet_main_initialize() mainInstance:onMainInitialize(); end 
function clinet_main_start() mainInstance:onMainStart(); end 
function client_main_tick() mainInstance:onMainUpdate(); end 
function client_main_stop() mainInstance:onMainUnInitialize(); end 

SystemExports[SystemFunctionIndex.CLIENT_LUA_MAIN_INITIALIZE] = clinet_main_initialize;
SystemExports[SystemFunctionIndex.CLIENT_LUA_MAIN_START] = clinet_main_start;
SystemExports[SystemFunctionIndex.CLIENT_LUA_MAIN_UPDATE] = client_main_tick;
SystemExports[SystemFunctionIndex.CLIENT_LUA_MAIN_STOP] = client_main_stop;