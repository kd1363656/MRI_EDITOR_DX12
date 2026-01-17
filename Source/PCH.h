#pragma once

//===============================================
// 基本
//===============================================
#pragma comment(lib , "winmm.lib")
#pragma comment(lib , "Rpcrt4.lib")

#define NOMINMAX
#include <Windows.h>
#include <stdio.h>

#include <wrl/client.h>

//===============================================
// "STL"
//===============================================
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <array>
#include <vector>
#include <stack>
#include <list>
#include <iterator>
#include <queue>
#include <algorithm>
#include <memory>
#include <random>
#include <fstream>
#include <iostream>
#include <sstream>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <future>
#include <filesystem>
#include <source_location>
#include <codecvt>

//===============================================
// 外部便利ライブラリ
//===============================================

#include "json.hpp"
#include "strconv2.h"