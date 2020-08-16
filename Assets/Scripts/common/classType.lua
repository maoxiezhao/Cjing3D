function IsLuaCustomClass(v)
	return type(v) == "table" and rawget(v, "__value_type") == "LUA_CUSTOM_CLASS";
end 

local GlobalClassWrapMetatable = {
	__newindex = function(t, k, v)
		if IsLuaCustomClass(v) then 
			v.__class_name = k;
		end 
	
		rawset(t, k, v);
	end,
}

setmetatable(_G, GlobalClassWrapMetatable);

NilMember = {};

-------------------------------------------------------------------------------------------
-- 设置class仅能在ctor中定义新成员变量(t[k] ~= nil)
-- 同时对于nil的成员变量，使用NilMember赋值
ClassBeforeCtorMetatable = {
	__index = function(t, k)
		-- return metatable func
		if ClassBeforeCtorMetatable[k] ~= nil then 
			return ClassBeforeCtorMetatable[k];
		end 
		return t.m_original_obj[k];
	end,

	__newindex = function(t, k, v)
		if v == nil then 
			v = NilMember;
		end 
		t.m_original_obj[k] = v;
	end,

	getOriginalMetatable = function(obj)
		return obj.m_original_metatable;
	end,

	-- 允许在ctor中重载__index和__newindex
	overrideIndexFuntion = function(obj, index_function, new_index_function)
		obj.m_original_obj.m_override_index_function = index_function;
		obj.m_original_obj.m_override_newindex_function = new_index_function;
	end, 
}

function CreateInstanceProxyBeforeCtor(obj)
	local proxy = {
		m_original_metatable = getmetatable(obj);
		m_original_obj = obj;
	};
	setmetatable(proxy, ClassBeforeCtorMetatable);
	return proxy;
end 

-------------------------------------------------------------------------------------------

ClassOverideIndexFunctionMetatable = {
	__index = function(t, k)
		local obj = t.m_original_obj;
		if obj.m_override_index_function ~= nil then 
			return obj:m_override_index_function(k);
		else
			return t.m_original_metatable:__index(k);
		end 
	end,

	__newindex = function(t, k, v)
		local obj = t.m_original_obj;
		if obj.m_override_newindex_function ~= nil then 
			return obj:m_override_newindex_function(k);
		else
			return t.m_original_metatable:__newindex(k);
		end 
	end,

	__eq = function(t1, t2)
		return t1.m_original_obj == t2.m_original_obj;
	end,
}

function CreateInstanceProxyWithOverridIndexFunction(obj)
	local proxy = {
		m_original_metatable = getmetatable(obj);
		m_original_obj = obj;
	};
	setmetatable(proxy, ClassOverideIndexFunctionMetatable);
	return proxy;
end 

-------------------------------------------------------------------------------------------

ClassMemberProtectedMetatable = {
	__index = function(t, k)
		if k == nil then 
			print(debug.traceback());
		end 

		local v = t.m_original_obj[k];
		if v == nil then 
			Logger.Error("__index:The member " .. k .." is not defined");
		elseif v == NilMember then 
			return nil;
		else
			return v;
		end 
	end,

	__newindex = function(t, k, v)
		if (t.m_original_obj[k] == nil) then 
			Logger.Error("__newindex:The member " .. k .." is not defined");
		else
			if v == nil then 
				v = NilMember;
			end 
			t.m_original_obj[k] = v;
		end 
	end,

	__eq = function(t1, t2)
		return t1.m_original_obj == t2.m_original_obj;
	end,
}

function CreateInstanceProxyWithMakeMemberProtected(obj)
	local proxy = {
		m_original_metatable = getmetatable(obj);
		m_original_obj = obj;
	};
	setmetatable(proxy, ClassMemberProtectedMetatable);
	return proxy;
end 

function CreateInstanceDestructor(obj, func)
	local meta = getmetatable(obj);
	if meta == nil or func == nil then 
		return;
	end 

	meta.__gc = func;
end 

-------------------------------------------------------------------------------------------

ClassObject = {};
ClassObject.__index = ClassObject;

function ClassObject:new()
	local instance = {};
	setmetatable(instance, ClassObject);
	return instance;
end 

function ClassObject:GetType()
	local metatable = getmetatable(self);
	if metatable == ClassMemberProtectedMetatable then 
		metatable = self.m_original_metatable;
	end 

	return metatable;
end 

local function has_base_type(type_def, base_type_def)
	if type_def == nil or base_type_def == nil then 
		return false;
	end 

	if type_def == base_type_def then 
		return true;
	end 

	return has_base_type(type_def.__base_class, base_type_def);
end 

function ClassObject:HasBaseType(base_type_def)
	return has_base_type(self:GetType(), base_type_def);
end 

function ClassObject:NewInstance(class_definition, ...)
	if class_definition == ClassObject then 
		return ClassObject:new();
	end 

	local new_instance = ClassObject:NewInstance(class_definition.__base_class, ...); --ClassObject:NewInstance(class_definition.__base_class, nil);
	setmetatable(new_instance, class_definition);

	--仅能在构造函数中定义变量，故在构造函数之前设置下元表
	local proxy = CreateInstanceProxyBeforeCtor(new_instance);
	class_definition.ctor(proxy, ...);

	if new_instance.m_override_index_function ~= nil or new_instance.m_override_newindex_function ~= nil then 
		new_instance = CreateInstanceProxyWithOverridIndexFunction(new_instance);
	end 

	CreateInstanceDestructor(new_instance, function(obj)
		if (class_definition.destor ~= nil) then 
			class_definition.destor(new_instance);
		end 

		local base_class = class_definition.__base_class;
		while(base_class ~= nil) do 
			if (base_class.destor ~= nil) then 
				base_class.destor(new_instance);
			end 

			base_class = base_class.__base_class;
		end 
	end);

	return new_instance;
end 

function NewClassDefinition(base_class_definition)
	if base_class_definition == nil then 
		base_class_definition = ClassObject;
	end 

	local new_class_definition = {};
	new_class_definition.__index = new_class_definition;
	new_class_definition.__base_class = base_class_definition;
	new_class_definition.__value_type = "LUA_CUSTOM_CLASS";
	new_class_definition.__class_name = ""; -- set lazy

	setmetatable(new_class_definition, base_class_definition)

	new_class_definition.new = function(obj, ...)
		local new_instance = ClassObject:NewInstance(new_class_definition, ...);
		return CreateInstanceProxyWithMakeMemberProtected(new_instance);
	end;

	new_class_definition.getTypeName = function()
		return new_class_definition.__class_name;
	end;

	return new_class_definition;
end 

function ClassDefinition(class_definition, base_class_definition)
	if class_definition ~= nil then 
		return;
	end 

	return NewClassDefinition(base_class_definition);
end 

-------------------------------------------------------------------------------------------

BaseNumber = NewClassDefinition(BaseNumber);
function BaseNumber:getTypeName()
	return "number";
end 

BaseString = NewClassDefinition(BaseString);
function BaseString:getTypeName()
	return "string";
end 

BaseBoolean = NewClassDefinition(BaseBoolean);
function BaseBoolean:getTypeName()
	return "boolean";
end 