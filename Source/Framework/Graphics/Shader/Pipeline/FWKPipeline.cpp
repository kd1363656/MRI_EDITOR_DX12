#include "FWKPipeline.h"

void FWK::Graphics::Pipeline::Init()
{
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


}