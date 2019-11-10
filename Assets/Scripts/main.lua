
Logger.Info("Load main file success.")

function OnMainInitialize()
	Logger.Info("OnMainInitialize")
end 

function OnMainUpdate()

end 

function OnMainUnInitialize()
	Logger.Info("OnMainUnInitialize")
end 

SystemExports[SystemFunctionIndex.CLIENT_LUA_MAIN_START] = OnMainInitialize;
SystemExports[SystemFunctionIndex.CLIENT_LUA_MAIN_UPDATE] = OnMainUpdate;
SystemExports[SystemFunctionIndex.CLIENT_LUA_MAIN_STOP] = OnMainUnInitialize;