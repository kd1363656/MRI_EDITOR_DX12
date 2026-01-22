#include "FWKPipeline.h"

void FWK::Graphics::Pipeline::Init(const std::vector<ComPtr<ID3DBlob>>& a_bufferList , const ComPtr<ID3D12RootSignature>& a_rootSignature)
{
	if (!a_rootSignature)
	{
		assert(false && "ルートシグネチャがまだ作成されていません。");
		return;
	}

	// レイアウトの定義
	D3D12_INPUT_ELEMENT_DESC l_inputLayout[] =
	{
		{ "POSITION"                                  ,
		   0U                                         ,
		   DXGI_FORMAT_R32G32B32_FLOAT                ,
		   0U                                         ,
		   D3D12_APPEND_ALIGNED_ELEMENT               ,
		   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA ,
		   0U
		},
	};


	D3D12_GRAPHICS_PIPELINE_STATE_DESC l_desc = {};

	// 後で設定
	l_desc.pRootSignature = a_rootSignature.Get();

	// シェーダーのセット
	l_desc.VS.pShaderBytecode = a_bufferList[0]->GetBufferPointer();
	l_desc.VS.BytecodeLength  = a_bufferList[0]->GetBufferSize   ();
	l_desc.PS.pShaderBytecode = a_bufferList[1]->GetBufferPointer();
	l_desc.PS.BytecodeLength  = a_bufferList[1]->GetBufferSize   ();

	// デフォルトのサンプルマスクを表す定数 (0xffffffff)
	l_desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// まだアンチエイリアスは使わないため"false"
	l_desc.RasterizerState.CullMode        = D3D12_CULL_MODE_NONE;	// カリングしない
	l_desc.RasterizerState.FillMode        = D3D12_FILL_MODE_SOLID;	// 中身を塗りつぶす
	l_desc.RasterizerState.DepthClipEnable = true;					// 深度方向のクリッピングは有効に

	// ブレンドモードの設定
	l_desc.BlendState.AlphaToCoverageEnable  = false;
	l_desc.BlendState.IndependentBlendEnable = false;

	D3D12_RENDER_TARGET_BLEND_DESC l_renderTargetBlendDesc = {};

	l_renderTargetBlendDesc.BlendEnable           = false;
	l_renderTargetBlendDesc.LogicOpEnable         = false;
	l_renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	
	l_desc.BlendState.RenderTarget[0] = l_renderTargetBlendDesc;

	// 入力レイアウトの設定
	l_desc.InputLayout.pInputElementDescs = l_inputLayout;	         // レイアウト戦闘アドレス
	l_desc.InputLayout.NumElements        = _countof(l_inputLayout); // レイアウト配列の要素数

	l_desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;	// カットなし

	// 三角形で構成
	l_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	l_desc.NumRenderTargets = 1;						// 今は一つのみ
	l_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;	// "0 ~ 1"に正規化された"RGBA"

	l_desc.SampleDesc.Count   = 1;	// サンプリングは"1"ピクセルにつき"1"
	l_desc.SampleDesc.Quality = 0;	// クオリティは最低

	ID3D12PipelineState* l_pipelineState = nullptr;

	const auto& l_graphicsManager = FWK::Graphics::GraphicsManager::GetInstance();
	auto		l_device		  = l_graphicsManager.GetDevice                ();

	if (!l_device)
	{
		assert(false && "デバイス作成ができていません。");
		return;
	}

	auto l_result = l_device->CreateGraphicsPipelineState(&l_desc , IID_PPV_ARGS(&l_pipelineState));

	if (l_result == E_INVALIDARG)
	{
		assert(false && "ルートシグネチャが実装されていないのでエラー");
	}
}
