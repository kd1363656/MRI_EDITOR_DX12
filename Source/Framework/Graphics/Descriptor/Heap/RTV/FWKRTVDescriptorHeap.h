#pragma once

namespace FWK::Graphics
{
	class RTVDescriptorHeap
	{
	public:

		RTVDescriptorHeap () = default;
		~RTVDescriptorHeap() = default;

		bool Init(UINT a_numDescriptors);

		UINT CreateRTV(const ComPtr<ID3D12Resource2>& a_resource);

	private:

		ComPtr<ID3D12DescriptorHeap> m_descriptorHeap = nullptr;

		UINT m_descriptorSize = 0U;
		UINT m_currentIndex   = 0U;
		UINT m_maxIndex       = 0U;
	};
}