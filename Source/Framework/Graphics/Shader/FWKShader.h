#pragma once

namespace FWK::Graphics
{
	class Shader final
	{
	public:

		Shader () = default;
		~Shader() = default;

		void Init();

	private:

		ComPtr<ID3DBlob> m_vsBlob = nullptr;
		ComPtr<ID3DBlob> m_psBlob = nullptr;

		std::unique_ptr<FWK::Graphics::Pipeline> m_pipeline = nullptr;
	};
}