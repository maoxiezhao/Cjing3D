#pragma once

#include "common\common.h"
#include "helper\IDGenerator.h"

#include <wrl.h>
#include <memory>
#include <vector>
#include <string>

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;