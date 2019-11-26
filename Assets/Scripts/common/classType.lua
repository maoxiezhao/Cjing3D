
-- simple class definition
function ClassDefine(classname)
	local cls = {}
	cls.__cname = classname
	cls.__index = cls
	if not cls.ctor then 
		cls.ctor = function() 
		end
	end
	
	cls.new = function(...)
		local curClass = cls
		local instance = {}
		setmetatable(instance, curClass)
		instance.class = curClass
		instance:ctor(...)
		return instance
	end
	
	return cls
end
