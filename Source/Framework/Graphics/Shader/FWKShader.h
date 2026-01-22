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

		ComPtr<ID3DBlob> vsBlob = nullptr;
		ComPtr<ID3DBlob> psBlob = nullptr;
	};
}