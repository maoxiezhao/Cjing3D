
require("common.classType")

-- simple version

local function check_key_type(key, target_type)
    local key_type = type(key);
    if key_type ~= "table" then 
        return target_type:getTypeName() == key_type;
    else
        if IsLuaCustomClass(target_type) then 
            return key:HasBaseType(target_type);
        else
            return false;
        end 
    end 
end 

--------------------------------------------------------------------------------------------

DynamicArray = ClassDefinition(DynamicArray);
function DynamicArray:ctor(value_type)
    if IsLuaCustomClass(value_type) == false then 
        Logger.Error("DynamicArray: invalid value type, excepted custom class.");
    end 

    self.m_value_type = value_type;
    self.m_value_table = {};

    self.m_origin_metatable = self:getOriginalMetatable();
    self:overrideIndexFuntion(DynamicArray.indexFunction, DynamicArray.newindexFunction);
end 

function DynamicArray:indexFunction(key)
    -- 返回ctor中赋值的成员变量
    local value = rawget(self, key);
    if value ~= nil then 
        return value;
    end 

    -- 返回类的定义成员函数
    local value = self.m_origin_metatable.__index[key];
    if value ~= nil then 
        return value;
    end 

    if check_key_type(key, BaseNumber) == false then 
        Logger.Warning("DynamicArray:__index: index is not a number");
        return;
    end 

    return self.m_value_table[key + 1];
end 

function DynamicArray:newindexFunction(key, value)
    if check_key_type(key, BaseNumber) == false then 
        Logger.Warning("DynamicArray:__newindex: index is not a number");
        return;
    end 
    
    if check_key_type(value, self.m_value_type) == false then 
        Logger.Warning("DynamicArray:__newindex: invalid value type");
        return;
    end 

    if key < 0 or key > (#self.m_value_table) then 
        Logger.Warning("DynamicArray:Get: index out of range");
        return;
    end 

    self.m_value_table[key + 1] = value;
end 

function DynamicArray:add(value)
    if check_key_type(value, self.m_value_type) == false then 
        Logger.Warning("DynamicArray:Add: invalid value type");
        return;
    end 

    table.insert(self.m_value_table, value);
end 

function DynamicArray:get(index)
    if check_key_type(index, BaseNumber) == false then 
        Logger.Warning("DynamicArray:Get: index is not a number");
        return;
    end 

    if index < 0 or index >= #self.m_value_table then 
        Logger.Warning("DynamicArray:Get: index out of range");
        return;
    end 

    return self.m_value_table[index + 1];
end 

function DynamicArray:size()
    return #self.m_value_table;
end 

function DynamicArray:removeAt(index)
    if check_key_type(index, BaseNumber) == false then 
        Logger.Warning("DynamicArray:Get: index is not a number");
        return;
    end 

    if table.remove(self.m_value_table, index + 1) == nil then 
        Logger.Warning("DynamicArray:Get: index out of range");
    end 
end 

function DynamicArray:iterator()
    local index = -1;
    local array = self;
    return function()
        index = index + 1;
        return array.m_value_table[index + 1];
    end;
end 

function DynamicArray:clear()
    self.m_value_table = {};
end 
--------------------------------------------------------------------------------------------
Map = ClassDefinition(Map);
function Map:ctor(key_type, value_type, convert_to_key_func)
    self.m_key_type = key_type;
    self.m_value_type = value_type;
    self.m_value_map = {};

    self.m_convert_to_key_func = convert_to_key_func;

    if IsLuaCustomClass(key_type) == false then 
        Logger.Error("DynamicArray: invalid key type, excepted custom class.");
    end 

    if IsLuaCustomClass(value_type) == false then 
        Logger.Error("DynamicArray: invalid value type, excepted custom class.");
    end 
end 

function Map:set(key, value)
    if check_key_type(key, self.m_key_type) == false then 
        Logger.Warning("Map:Set(): invalid key type");
        return;
    end 

    if check_key_type(value, self.m_value_type) == false then 
        Logger.Warning("Map:Set(): invalid value type");
        return;
    end 

    self.m_value_map[key] = value;
end 

function Map:get(key)
    if key == nil then 
        return;
    end 

    if check_key_type(key, self.m_key_type) == false then 
        Logger.Warning("Map:get(): invalid key type");
        return;
    end 

    return self.m_value_map[key];
end 

function Map:delete(key)
    if check_key_type(key, self.m_key_type) == false then 
        Logger.Warning("Map:delete(): invalid key type");
        return;
    end 

    self.m_value_map[key] = nil;
end 

function Map:size()
    return #self.m_value_map;
end 

function Map:clear()
    self.m_value_map = {};
end 

function Map:iterator()
    return pairs(self.m_value_map);
end 