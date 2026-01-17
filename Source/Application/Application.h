#pragma once

class Application final : public FWK::SingletonBase<Application>
{
public:

	void Execute();

private:

	bool Init(const FWK::CommonStruct::Dimension2D& a_size);

	void LoadWindowSize();
	void SaveWindowSize();

	const std::string k_windowSizeFileIOPath = "Asset/Data/Setting/Window/WindowConfig.json";

	FWK::Window m_window = {};

	FWK::CommonStruct::Dimension2D m_windowSize = FWK::CommonConstant::k_defaultWindowSize;

	//===============================
	// ƒVƒ“ƒOƒ‹ƒgƒ“
	//===============================
	friend class FWK::SingletonBase<Application>;

	Application ()          = default;
	~Application() override = default;
};