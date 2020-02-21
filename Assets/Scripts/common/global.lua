require("common.common")
require("core.addonManager")

global_context = {
    m_input_manager = System.GetInputManager();
    m_addon_manager = AddonManager:new();
};


return global_context;