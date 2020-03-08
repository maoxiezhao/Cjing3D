#pragma once

#include <d3d11_3.h>
#include <DXGI1_3.h>

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"Dxgi.lib")
#pragma comment(lib,"dxguid.lib")

// 是否在结束时打印d3d对象信息
#define DEBUG_REPORT_LIVE_DEVICE_OBJECTS