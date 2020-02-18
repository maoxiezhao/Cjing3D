require("common.common")
require("ui.baseAddon")

TestAddon = ClassDefinition(TestAddon, BaseAddon);
function TestAddon:ctor()
    self.m_root_widget = nil;
    self.m_is_initialized = false;
    self.m_is_visible = false;
end 

function TestAddon:initialize()
    local root_widget = nil; --Gui.LoadWidgetFromXML(self.m_name, "UI/test.xml", self);

    -- get children

    self.m_root_widget = root_widget;
    self.m_is_initialized = true;
end 

function TestAddon:setVisible(is_visible)
    if self.m_is_visible ~= is_visible then 
        root_widget:SetVisible(is_visible);
        self.m_is_visible = is_visible;
    end 
end 

function TestAddon:clear()
    if self.m_root_widget ~= nil then 
        self.m_root_widget:ClearSelf();
    end 

    self.m_root_widget = nil;
    self.m_is_visible = false;
    self.m_is_initialized = false;
end 

---------------------------------------------------------

function TestAddon:onLoaded(sender, variantArray)
    print("AAA!!!!!")
end 