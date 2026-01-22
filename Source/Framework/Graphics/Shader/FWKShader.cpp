#include "FWKShader.h"

// ピクセルシェーダー、バーテックスシェーダーの読み込み
void FWK::Graphics::Shader::Init()
{
	ID3DBlob* l_errorBlob = nullptr;

	auto l_hr = D3DCompileFromFile(L"Asset/Data/Shader/Test/TestShader_VS.hlsl"    ,	// シェーダー名
								   nullptr                                         ,	// "define"はなし
								   D3D_COMPILE_STANDARD_FILE_INCLUDE               ,	// インクルードはデフォルト
								   "TestVS"							               ,	// 関数は"TestVS"
								   "vs_5_0"							               ,	// 対象シェーダーは"vs_5_0"
								   D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION ,	// デバック用及び最適化なし
								   0U											   ,	
								   &m_vsBlob									   , 
								   &l_errorBlob);										// エラー時には"l_errorBlob"にメッセージが入る	

	if (FAILED(l_hr))
	{
		// ファイルが見当たらなかったら実行
		if(l_hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			OutputDebugStringW(L"ファイルが見当たりません");
		}
		else
		{
			auto l_errorSTR = std::string();

			l_errorSTR.resize(l_errorBlob->GetBufferSize());

			std::copy_n((char*)l_errorBlob->GetBufferPointer() , 
						 l_errorBlob->GetBufferSize         () ,
						 l_errorSTR.begin                   ());

			l_errorSTR += "\n";

			OutputDebugStringW(sjis_to_wide(l_errorSTR).c_str());
		}

		assert(false && "頂点シェーダーのファイル読み込みに失敗。");
		return;
	}

	l_hr = D3DCompileFromFile(L"Asset/Data/Shader/Test/TestShader_PS.hlsl"    ,	// シェーダー名
							  nullptr                                         ,	// "define"はなし
							  D3D_COMPILE_STANDARD_FILE_INCLUDE               ,	// インクルードはデフォルト
							  "TestPS"							              ,	// 関数は"TestVS"
							  "ps_5_0"							              ,	// 対象シェーダーは"vs_5_0"
							  D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION ,	// デバック用及び最適化なし
							  0U											  ,	
							  &m_psBlob									      , 
							  &l_errorBlob);										// エラー時には"l_errorBlob"にメッセージが入る	

	if (FAILED(l_hr))
	{
		// ファイルが見当たらなかったら実行
		if(l_hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			OutputDebugStringW(L"ファイルが見当たりません");
		}
		else
		{
			auto l_errorSTR = std::string();

			l_errorSTR.resize(l_errorBlob->GetBufferSize());

			std::copy_n((char*)l_errorBlob->GetBufferPointer() , 
						 l_errorBlob->GetBufferSize         () ,
						 l_errorSTR.begin                   ());

			l_errorSTR += "\n";

			OutputDebugStringW(sjis_to_wide(l_errorSTR).c_str());
		}

		assert(false && "ピクセルシェーダーのファイル読み込みに失敗。");
		return;
	}
}