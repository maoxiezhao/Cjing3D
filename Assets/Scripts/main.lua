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
	Logger.Info("OnMainInitialize")

	collectgarbage('setpause', 200)
	collectgarbage('setstepmul', 400)

	local main_camera = Render.GetMainCamera();
	self.m_fps_camera:setCamera(main_camera);
	self.m_fps_camera:reset(Vector:new(0, 0, -10));
end 

function MainInstance:updateCamera(deltaTime)
	self.m_fps_camera:update(deltaTime);
end 

function MainInstance:onMainUpdate()
	-- 处理引用mem leak
	-- local input_manager = System.GetInputManager();

	local deltaTime = System.GetDeltaTime();
	-- tick begin

	print();
	--collectgarbage("collect")
	print("COUNT_1:", collectgarbage("count"))
	self:updateCamera(deltaTime);
	print("COUNT_2:", collectgarbage("count"))
	--collectgarbage("collect")
end 

function MainInstance:onMainUnInitialize()
	Logger.Info("OnMainUnInitialize")
end 

local mainInstance = MainInstance:new();
function clinet_main_start() mainInstance:onMainInitialize(); end 
function client_main_tick() mainInstance:onMainUpdate(); end 
function client_main_stop() mainInstance:onMainUnInitialize(); end 

SystemExports[SystemFunctionIndex.CLIENT_LUA_MAIN_START] = clinet_main_start;
SystemExports[SystemFunctionIndex.CLIENT_LUA_MAIN_UPDATE] = client_main_tick;
SystemExports[SystemFunctionIndex.CLIENT_LUA_MAIN_STOP] = client_main_stop;