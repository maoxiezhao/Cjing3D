require("common.common")
require("core.addonManager")
require("core.sceneManager")

global_context = {
    m_input_manager = System.GetInputManager();
    m_addon_manager = AddonManager:new();
    m_scene_manager = SceneManager:new();
};


return global_context;