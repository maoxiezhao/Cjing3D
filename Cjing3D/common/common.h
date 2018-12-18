#pragma once

#include <memory>
#include <algorithm>
#include <string>

#include "helper\logger.h"
#include "helper\debug.h"
#include "helper\stringID.h"
#include "utils\math.h"

#undef min
#undef max

#ifdef _MSC_VER
#pragma warning( disable :4005 4018 4522 4715 4800 4996)
#endif

#define SAFE_DELETE(p) if(p!= nullptr) {delete (p); p=nullptr;}
#define SAFE_DELETE_ARRAY(p) if(p!= nullptr) {delete[](p); p=nullptr;}

#define CJING_DEBUG