require("common.common")
require("ui.baseAddon")
require("ui.test")

AddonManager = ClassDefinition(AddonManager);
function AddonManager:ctor()
    self.m_loaded_ui_addons = Map:new(BaseString, BaseAddon);
end 

function AddonManager:initialize()
    Logger.Info("Addonmanager initialized.")
    
    self:loadAllUITemplates();
    self:loadAllUIAddons();
end 

function AddonManager:uninitialize()
    Logger.Info("Addonmanager uninitialized.")

    for _, addon in self.m_loaded_ui_addons:iterator() do 
        addon:clear();
    end 
end 

function AddonManager:loadUIAddonByPath(name, path)
    Logger.Info("Addonmanager loadUIAddonByPath:" .. path);

    local addon = self.m_loaded_ui_addons:get(name);
    if addon ~= nil then 
        Logger.Warning("Addon "..  name .. " has already loaded.")
        return;
    end 

    local addon, err = TRACEBACK_PCALL(TRACE_LOAD_FILE, nil, path)
	if not ret then 
		Logger.Warning("Addon "..  name .. " load failed." .. err)
        return;
    end
    addon:setName(STRING_ID(name));

    if addon.initialize ~= nil then 
        TRACEBACK_PCALL(addon.initialize, addon);
    end 

    self.m_loaded_ui_addons:set(name, addon);

    return addon;
end 

function AddonManager:loadUIAddonByClass(name, class_definition)
    Logger.Info("Addonmanager loadUIAddonByClass:" .. class_definition:getTypeName());

    local addon = self.m_loaded_ui_addons:get(name);
    if addon ~= nil then 
        Logger.Warning("Addon "..  name .. " has already loaded.")
        return;
    end 

    local addon = class_definition:new();
    addon:setName(STRING_ID(name));

    if addon.initialize ~= nil then 
        TRACEBACK_PCALL(addon.initialize, addon);
    end 
    self.m_loaded_ui_addons:set(name, addon);

    return addon;
end 

function AddonManager:loadAllUITemplates()

end 

function AddonManager:loadAllUIAddons()
    Logger.Info("******************** AddonManager: start Loading all ui addons ********************")

    self:loadUIAddonByClass("test", TestAddon);

    Logger.Info("******************** AddonManager: finish Loading all ui addons ********************")
end 