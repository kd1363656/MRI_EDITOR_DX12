#pragma once

namespace FWK::FileIOUtility
{
	static constexpr int k_jsonIndentNum = 4;

	inline nlohmann::json LoadJsonFile(const std::string& a_filePath)
	{
		std::filesystem::path l_path(a_filePath);
	
		// パスが通常ファイルであることを確認
		if (!std::filesystem::is_regular_file(l_path))
		{
			return nlohmann::json();
		}

		// ファイル読み込み
		std::ifstream l_ifs(l_path);
		if (l_ifs.fail())
		{
			return nlohmann::json(); 
		}

		// ファイルの全体のバッファを読み込む
		std::stringstream l_buffer;
		l_buffer << l_ifs.rdbuf();
		l_ifs.close			   ();

		// "ifs"で読み込んだファイルが空かチェック
		if (l_buffer.str().empty())
		{
			return nlohmann::json();
		}

		// 例外をスローせず、コールバックなしで"json"をパース
		auto l_rootJson = nlohmann::json::parse(l_buffer.str() , nullptr , false);

		// パースに失敗していないか確認
		if (l_rootJson.is_discarded())
		{
			return nlohmann::json();
		}

		return l_rootJson;
	}

	inline void SaveJsonFile(const nlohmann::json& a_json , const std::string& a_filePath)
	{
		std::filesystem::path l_path(a_filePath);

		// 保存時、ファイルパス末尾に".json"拡張子が抜けていれば足してあげる
		if (l_path.extension() != FWK::CommonConstant::k_jsonExtension)
		{
			l_path += FWK::CommonConstant::k_jsonExtension;
		}

		std::ofstream l_ofs(l_path , std::ios::out);

		l_ofs << a_json.dump(FWK::FileIOUtility::k_jsonIndentNum);
		l_ofs.close			();
	}

	inline bool DeleteFileByPath(const std::string& a_filePath)
	{
		std::filesystem::path l_path(a_filePath);

		// 指定されたパスが通常のファイかを確認
		if (!std::filesystem::is_regular_file(l_path))
		{
			return false;
		}

		// 削除を試みる
		return std::filesystem::remove(l_path);
	}

	inline bool CreateEmptyFile(const std::string& a_filePath)
	{
		std::ofstream l_ofs(a_filePath , std::ios::out | std::ios::trunc);
		return !l_ofs.fail ();
	}
}