require("common.common")

BaseAddon = ClassDefinition(BaseAddon);
function BaseAddon:ctor()
    self.m_name = STRING_ID_EMPTY;
end 

function BaseAddon:initialize()
end 

function BaseAddon:clear()
end 

function BaseAddon:setName(name)
    self.m_name = name;
end 

function BaseAddon:getName()
    return self.m_name;
end 
