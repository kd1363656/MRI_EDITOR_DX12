#pragma once

namespace FWK::CommonConstant
{
	static constexpr FWK::CommonStruct::Dimension2D k_defaultWindowSize = { 1280U , 720U };

	static constexpr std::string_view k_jsonExtension = ".json";

	static constexpr UINT k_defaultBackBufferNum = 2U;
	static constexpr UINT k_singleGPUNodeMask    = 0U;	// 単一"GPU"を使用するノードマスク。マルチアダプター非対応。
}