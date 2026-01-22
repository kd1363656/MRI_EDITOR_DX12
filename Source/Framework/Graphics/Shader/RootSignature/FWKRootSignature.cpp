#include "FWKRootSignature.h"

void FWK::Graphics::RootSignature::Init()
{
	D3D12_ROOT_SIGNATURE_DESC l_rootSignatureDesc = {};

	l_rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob*        l_errorBlob = nullptr;
	ComPtr<ID3DBlob> l_rootBlob  = nullptr;

	auto l_hr = D3D12SerializeRootSignature(&l_rootSignatureDesc           ,	// パラメータ
											D3D_ROOT_SIGNATURE_VERSION_1_0 ,	// バージョン
											&l_rootBlob                    ,
											&l_errorBlob);

	if (FAILED(l_hr))
	{
		assert(false && "ルートシグネチャ作成失敗");
		return;
	}

	// ルートシグネチャオブジェクトの作成

	const auto& l_graphicsManager = FWK::Graphics::GraphicsManager::GetInstance();
	auto		l_device          = l_graphicsManager.GetDevice				   ();;

	if (!l_device)
	{
		assert(false && "デバイスが作成されていません。");
		return;
	}

	l_hr = l_device->CreateRootSignature(0U                             , // 単一アダプターとして設定 
										 l_rootBlob->GetBufferPointer() , 
										 l_rootBlob->GetBufferSize   () ,
										 IID_PPV_ARGS(&m_rootSignature));

	if (FAILED(l_hr))
	{
		assert(false && "ルートシグネチャ作成失敗");
		return;
	}
}