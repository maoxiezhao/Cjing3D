require("scene.scene")
require("utils.camera.fpsCamera")

EditorScene = ClassDefinition(EditorScene, GameScene);
function EditorScene:ctor()
    self.m_fps_camera = FPSCamera:new();
    self.m_scene_inst = nil;
end 

function EditorScene:initialize()
end 

function EditorScene:onStart()
	-- camera
	local main_camera = Render.GetMainCamera();
	self.m_fps_camera:setCamera(main_camera);
	self.m_fps_camera:reset(Vector:new(0, 10, -5), Vector:new(1.2, 0, 0));
	self.m_fps_camera:setMoveSpeed(20);
	self.m_fps_camera:setRotateSpeed(0.005);
	
	-- scene add custom light
    local scene = Scene.GetMainScene();
    self.m_scene_inst = scene;

    -- default light
    self.m_scene_inst = Scene:GetMainScene();
    SceneHelper.createDirectionLight(self.m_scene_inst, 0.8, Vector:new(0, 0, 0));
end 

function EditorScene:onStop()

end 

function EditorScene:update(deltaTime)
	self.m_fps_camera:update(deltaTime);
end 