
Logger.Info("Load main file success.")

function OnMainInitialize()
	Logger.Info("OnMainInitialize")
end 

function OnMainUpdate()
	local input_manager = System.GetInputManager();
	if input_manager:IsKeyDown(KeyCode.Space) == true then 
		print("Space key down!");
	end 

end 

function OnMainUnInitialize()
	Logger.Info("OnMainUnInitialize")
end 

SystemExports[SystemFunctionIndex.CLIENT_LUA_MAIN_START] = OnMainInitialize;
SystemExports[SystemFunctionIndex.CLIENT_LUA_MAIN_UPDATE] = OnMainUpdate;
SystemExports[SystemFunctionIndex.CLIENT_LUA_MAIN_STOP] = OnMainUnInitialize;