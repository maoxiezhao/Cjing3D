require("common.common")
require("scene.scene")

SceneManager = ClassDefinition(SceneManager);
function SceneManager:ctor()
    self.m_current_scene = nil;
    self.m_next_scene = nil;
end 

function SceneManager:initialize()
    Logger.Info("SceneManager initialized.")
end 

function SceneManager:uninitialize()
    Logger.Info("SceneManager uninitialize.")
end 

function SceneManager:update(deltaTime)
    if (self.m_current_scene ~= nil) then 
        self.m_current_scene:update(deltaTime);
    end 

    if (self.m_next_scene ~= nil) then 
        if (self.m_current_scene ~= nil) then 
            self.m_current_scene:onStop();
        end 

        self.m_current_scene = self.m_next_scene;
        self.m_next_scene = nil;

        if (self.m_current_scene ~= nil) then 
            self.m_current_scene:onStart();
        end 
    end 
end 

function SceneManager:setNextScene(scene)
    self.m_next_scene = scene;
end 
