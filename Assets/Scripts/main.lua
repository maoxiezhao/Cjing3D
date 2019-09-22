print("[LUA] Load main file success.")

function OnMainInitialize()
	print("[LUA] OnMainInitialize")
end 

function OnMainUpdate()

end 

function OnMainUnInitialize()
	print("[LUA] OnMainUnInitialize")
end 

SystemExports[SystemFunctionIndex.CLIENT_LUA_MAIN_START] = OnMainInitialize;
SystemExports[SystemFunctionIndex.CLIENT_LUA_MAIN_UPDATE] = OnMainUpdate;
SystemExports[SystemFunctionIndex.CLIENT_LUA_MAIN_STOP] = OnMainUnInitialize;