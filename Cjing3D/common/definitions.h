#pragma once

#undef min
#undef max

#ifdef _MSC_VER
#pragma warning( disable :4005 4018 4522 4715 4800 4996 4267)
#endif

#define SAFE_DELETE(p) if(p!= nullptr) {delete (p); p=nullptr;}
#define SAFE_DELETE_ARRAY(p) if(p!= nullptr) {delete[](p); p=nullptr;}

#define CJING_DEBUG
#define CJING_IMGUI_ENABLE

// lua binding
#define LUA_BINDER_REGISTER_CLASS 
#define LUA_BINDER_REGISTER_CLASS_CONSTRUCTOR
#define LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
#define LUA_BINDER_REGISTER_CLASS_STATIC_FUNCTION
#define LUA_BINDER_REGISTER_CLASS_SET_NAME(name)