#include "FWKRTVDescriptorHeap.h"

bool FWK::Graphics::RTVDescriptorHeap::Init(UINT a_numDescriptors)
{
	const auto& l_device = FWK::Graphics::GraphicsManager::GetInstance().GetDevice();

	if (!l_device)
	{
		assert(false && "デバイスの作製が成功していません。");
		return false;
	}

	// ディスクリプタヒープ作成用パラメータ
	D3D12_DESCRIPTOR_HEAP_DESC l_desc = {};
	l_desc.NumDescriptors             = a_numDescriptors;						  // ヒープ内の記述子の数
	l_desc.Type						  = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;			  // ヒープの種類(レンダーターゲットビュー)
	l_desc.Flags					  = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;		  // ヒープオプション
	l_desc.NodeMask					  = FWK::CommonConstant::k_singleGPUNodeMask; // アダプターの数(単一想定)

	// 第一引数 : パラメータ、第二引数 : 取得するインターフェースの"ID"と、結果を受け取るポインタ
	if (FAILED(l_device->CreateDescriptorHeap(&l_desc , IID_PPV_ARGS(&m_descriptorHeap))))
	{
		assert(false && "\"RTV\"ディスクリプタヒープの作成に失敗しました。");
		return false;
	}

	// 指定されたディスクリプタタイプの一つ分のサイズ(インクリメントサイズ)を取得
	m_descriptorSize = l_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_maxIndex       = a_numDescriptors;
	m_currentIndex   = 0U;

	return true;
}

UINT FWK::Graphics::RTVDescriptorHeap::CreateRTV(const ComPtr<ID3D12Resource2>& a_resource)
{
	if (!m_descriptorHeap)
	{
		assert(false && "デスクリプタヒープの作製ができていません");
		return 0U;
	}

	const auto& l_device = FWK::Graphics::GraphicsManager::GetInstance().GetDevice();

	if (!l_device)
	{
		assert(false && "デバイスの作製が成功していません。");
		return 0U;
	}

	assert(m_currentIndex < m_maxIndex && "\"RTV\"ヒープに空きがありません。");

	auto l_handle = m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();	// ヒープの先端を表す"CPU"記述子ハンドル
	l_handle.ptr += static_cast<SIZE_T>(m_currentIndex) * m_descriptorSize;	// ヒープのディスクリプタサイズを今の要素数と書けることで配列を進める

	// 第一引数 : 作製対処のリソース、第二引数 : パラメータアドレス("nullptr"ならデフォルト設定が代入される、第三引数 : 書き込む"RTV"ディスクリプタの位置) 
	l_device->CreateRenderTargetView(a_resource.Get() , nullptr , l_handle);

	return m_currentIndex++;	// 使用済みのインデックスを返す
}

D3D12_CPU_DESCRIPTOR_HANDLE FWK::Graphics::RTVDescriptorHeap::GetRTVCPUHandle(UINT a_index) const
{
	if (!m_descriptorHeap || a_index >= m_maxIndex)
	{
		assert(false && "でスクリプタヒープの作製かヒープ領域を超えています。");
		return D3D12_CPU_DESCRIPTOR_HANDLE();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE l_handle = m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	l_handle.ptr += static_cast<SIZE_T>(a_index) * m_descriptorSize;

	return l_handle;
}