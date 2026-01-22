#include "FWKMesh.h"

void FWK::Graphics::Mesh::Create()
{
	m_vertices[0] = { -1.0F , -1.0F , 0.0F };
	m_vertices[1] = { -1.0F ,  1.0F , 0.0F };
	m_vertices[2] = {  1.0F , -1.0F , 0.0F };

	// ヒーププロパティのパラメータ
	D3D12_HEAP_PROPERTIES l_heapProp = {};
	l_heapProp.Type                  = D3D12_HEAP_TYPE_UPLOAD;			         // ヒープの種類を指定
	l_heapProp.CPUPageProperty       = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;          // "CPU"のページング設定
	l_heapProp.MemoryPoolPreference  = D3D12_MEMORY_POOL_UNKNOWN;		         // メモリプールがどこを示すか
	l_heapProp.CreationNodeMask      = FWK::CommonConstant::k_singleGPUNodeMask;	 // 単一アダプターとして設定
	l_heapProp.VisibleNodeMask       = FWK::CommonConstant::k_singleGPUNodeMask;	 // リソースが表示されるノード(単一アダプター)

	D3D12_RESOURCE_DESC l_resDesc = {};

	l_resDesc.Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER;
	l_resDesc.Width            = sizeof(m_vertices);					// リソースの幅
	l_resDesc.Height           = 1U;								// リソースの高さ
	l_resDesc.DepthOrArraySize = 1U;								// リソースの深さ(深度)
	l_resDesc.MipLevels        = 1U;								// "MIP"レベル
	l_resDesc.Format		   = DXGI_FORMAT_UNKNOWN;				// 画像でないので"UNKNOWN"
	l_resDesc.SampleDesc.Count = 1U;								// ピクセルあたりのマルチサンプル数
	l_resDesc.Flags            = D3D12_RESOURCE_FLAG_NONE;			// オプションフラグ
	l_resDesc.Layout           = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;		// レイアウト

	const auto& l_graphicsDeviceManager = FWK::Graphics::GraphicsManager::GetInstance();
	auto		l_device				= l_graphicsDeviceManager.GetDevice          ();

	if (!l_device) { return; }

	auto l_hr = l_device->CreateCommittedResource(&l_heapProp                       , 
												  D3D12_HEAP_FLAG_NONE              , 
												  &l_resDesc                        ,
												  D3D12_RESOURCE_STATE_GENERIC_READ ,
												  nullptr                           ,
												  IID_PPV_ARGS(&m_vertexBuffer));

	// 戻り値とポインタの中身を確認
	if (FAILED(l_hr) || !m_vertexBuffer)
	{
		assert(false && "頂点バッファー作成失敗");
		return;
	}

	// 頂点座標格納用ポインタ
	Math::Vector3* l_vertexMap = nullptr;

	// マップ登録開始
	l_hr = m_vertexBuffer->Map(0U , nullptr , (void**)&l_vertexMap);

	// マップに座標を格納していく
	std::copy(std::begin(m_vertices) , std::end(m_vertices) , l_vertexMap);

	// マップ登録をやめる
	m_vertexBuffer->Unmap(0 , nullptr);

	// ビューの作成
	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();	// バッファーの仮想アドレス
	m_vertexBufferView.SizeInBytes    = sizeof(m_vertices);						// 全バイト数
	m_vertexBufferView.StrideInBytes  = sizeof(m_vertices[0]);					// 一頂点当たりのバイト数
}