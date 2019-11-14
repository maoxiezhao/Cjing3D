package.path = package.path .. ";../Assets/Scripts/?.lua"

require("common.common")
require("common.global")

Logger.Info("Load main file success.")

local MainInstance = ClassDefine(MainInstance);
function MainInstance:ctor()
	self.m_main_camera = nil;
	self.m_camera_transform = Transform:new();
	self.m_camera_position = Vector:new(0, 0, -400);
end 

function MainInstance:onMainInitialize()
	Logger.Info("OnMainInitialize")

	collectgarbage('setpause', 200)
	collectgarbage('setstepmul', 200)

	self.m_main_camera = Render.GetMainCamera();
end 

function MainInstance:onMainUpdate()
	-- 处理引用mem leak
	-- local input_manager = System.GetInputManager();

	local is_pos_dirty = false;
	local input_manager = global_context.m_input_manager;
	if input_manager:IsKeyDown(KeyCode.W) == true then 
		local x = self.m_camera_position:GetZ();
		self.m_camera_position:SetZ(x + 20);
		is_pos_dirty = true;

	elseif input_manager:IsKeyDown(KeyCode.A) == true then 
		local x = self.m_camera_position:GetY();
		self.m_camera_position:SetY(x + 1);
		is_pos_dirty = true;

	elseif input_manager:IsKeyDown(KeyCode.S) == true then 
		local x = self.m_camera_position:GetZ();
		self.m_camera_position:SetZ(x - 20);
		is_pos_dirty = true;

	elseif input_manager:IsKeyDown(KeyCode.D) == true then 
		local x = self.m_camera_position:GetY();
		self.m_camera_position:SetY(x - 1);
		is_pos_dirty = true;

	end 

	if is_pos_dirty == true then 
		self.m_camera_transform:Clear();
		self.m_camera_transform:Translate(self.m_camera_position);
		self.m_camera_transform:Update();
		
		self.m_main_camera:Transform(self.m_camera_transform);
		self.m_main_camera:Update();
	end 
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