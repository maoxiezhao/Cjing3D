require("common.common")
require("camera")

MainScene = ClassDefinition(MainScene);
function MainScene:ctor()
    self.m_fps_camera = FPSCamera:new();
    self.m_scene_scene = nil;
end 

function MainScene:initialize()
end 

function MainScene:onStart()
	-- camera
	local main_camera = Render.GetMainCamera();
	self.m_fps_camera:setCamera(main_camera);
	self.m_fps_camera:reset(Vector:new(0, 5, -10), Vector:new(0.2, 0, 0));
	self.m_fps_camera:setMoveSpeed(20);
	self.m_fps_camera:setRotateSpeed(0.0005);
	
	-- scene add custom light
	local scene = Scene.GetMainScene();
	-- local entity = scene:CreateEntity();
	-- local light = scene:CreateLight(entity);
	-- light:SetRange(100);
	-- light:SetEnergy(20);

	-- local transform = scene:CreateTransform(entity);
    -- transform:Translate(Vector:new(0, 10, -18));
	-- transform:Update();
	--------------------------

	self.m_scene_scene = scene;
end 

function MainScene:uninitialize()

end 

function MainScene:update(deltaTime)
	self.m_fps_camera:update(deltaTime);
end 