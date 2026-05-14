#pragma once

namespace FileSystem
{
	nlohmann::json LoadJsonFile  (const std::string& FilePath);
	void           SaveJsonFile  (const nlohmann::json& Json, const std::string& FilePath);
	void           CreateJsonFile(const std::string& FilePath);

	std::vector<std::filesystem::path> GetAllFilePathsInDirectory(const std::string& DirectoryPath);

#pragma region Variable
	static const std::string JSON_EXTENSION = ".json";
	static const std::string SLASH_SYMBOL   = "/";

#pragma endregion
}