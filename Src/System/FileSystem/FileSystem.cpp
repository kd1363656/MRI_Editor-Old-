#include "FileSystem.h"

nlohmann::json FileSystem::LoadJsonFile(const std::string& FilePath)
{
	std::filesystem::path path_(FilePath);
	if (path_.extension() != JSON_EXTENSION)
	{
		path_ += JSON_EXTENSION;
	}

	// ファイル存在確認
	if (!std::filesystem::exists(path_))
	{
		KdDebugGUI::Instance().AddLog("\nJson file is not search\n");
		return nlohmann::json();
	}

	std::ifstream ifs_(path_);

	if (ifs_.fail())
	{
		KdDebugGUI::Instance().AddLog("\nJson file load is Failed\n");
		return nlohmann::json(); 
	}

	// ファイルの全体のバッファを読み込む
	std::stringstream buffer_;
	buffer_ << ifs_.rdbuf();
	ifs_.close();

	// "ifs"で読み込んだファイルが空かチェック
	if (buffer_.str().empty())
	{
		KdDebugGUI::Instance().AddLog("\nJson file is empty\n");
		return nlohmann::json();
	}

	auto json_ = nlohmann::json::parse(buffer_.str() , nullptr , false);

	// パースに失敗していないか確認
	if (json_.is_discarded())
	{
		KdDebugGUI::Instance().AddLog("\nJson parse error : invalid json structure\n");
		return nlohmann::json();
	}

	return json_;
}

void FileSystem::SaveJsonFile(const nlohmann::json& Json, const std::string& FilePath)
{
	std::filesystem::path path_(FilePath);

	if (path_.extension() != JSON_EXTENSION)
	{
		path_ += JSON_EXTENSION;
	}

	std::ofstream ofs_(path_, std::ios::out);

	ofs_ << Json.dump(4);
	ofs_.close();
}

// ファイルが存在しないなら空の"Json"ファイルを作る
void FileSystem::CreateJsonFile(const std::string& FilePath)
{
	std::filesystem::path path_(FilePath);

	if (path_.extension() != JSON_EXTENSION)
	{
		path_ += JSON_EXTENSION;
	}

	if (!std::filesystem::exists(path_))
	{
		std::ofstream ofs_(path_, std::ios::out);

		auto json_ = nlohmann::json();

		ofs_ << json_.dump(4);
		ofs_.close();
	}
}

std::vector<std::filesystem::path> FileSystem::GetAllFilePathsInDirectory(const std::string& DirectoryPath)
{
	std::vector<std::filesystem::path> filePath_;

	// "DirectoryPath"が存在するか、またそれはディレクトリであるかを確認
	if(!std::filesystem::exists(DirectoryPath) || !std::filesystem::is_directory(DirectoryPath))
	{
		KdDebugGUI::Instance().AddLog("指定されたパスは有効なディレクトリではありません");
		return filePath_;
	}

	// もしディレクトリであることが確認できればディレクトリ以下のファイルパスを保存
	for(const auto& entry_ : std::filesystem::recursive_directory_iterator(DirectoryPath))
	{
		if(std::filesystem::is_regular_file(entry_.path()))
		{
			filePath_.emplace_back(entry_.path());
		}
	}

	return filePath_;
}