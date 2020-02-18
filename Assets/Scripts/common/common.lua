
require("common.classType")
require("common.container")

local _xpcall = xpcall
local _debug_traceback = debug.traceback

local function msg_hander(msg)
    return _debug_traceback(msg, 2)
end

function TRACEBACK_PCALL(func, ...)
	return _xpcall(func, msg_hander, ...)
end

function TRACE_LOAD_FILE(path, env, ...)
    if not FileData.Exists(path) then
        error('File '..script..' not exists.'); 
        return;
    end
    
    local buf = FileData.Read(path);
    local f,err = load(buf, path, 'bt', env or _ENV);
    if f==nil then
        error(err);
        return;
    end
    return f(...);
end 

local string_id_string_map = {};
function STRING_ID(str)
    local string_id = string_id_string_map[str];
    if string_id == nil then 
        string_id = StringID:new(str);
        string_id_string_map[str] = string_id;
    end 

    return string_id;
end 

STRING_ID_EMPTY = STRING_ID("");