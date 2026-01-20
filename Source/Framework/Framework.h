#pragma once

// 定義
#include "Definition/Struct/FWKCommonStruct.h"
#include "Definition/Constant/FWKCommonConstant.h"

// シングルトンを安全に運用するための基底クラス
#include "Singleton/FWKSingletonBase.h"

// "FPS"計測クラス
#include "FPS/FWKFPSController.h"

// ウィンドウ
#include "Window/FWKWindow.h"

// ファイル入出力便利関数
#include "Utility/FileIO/FWKFileIOUtility.h"

// "RTV"ヒープb
#include "Graphics/Descriptor/Heap/RTV/FWKRTVDescriptorHeap.h"

// "Graphic"クラス
#include "Graphics/FWKGraphicsManager.h"