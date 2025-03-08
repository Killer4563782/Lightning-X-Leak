#pragma once
#include <filesystem>

namespace folder_helpers
{
	inline fs::path get_folder_path(const KNOWNFOLDERID* folder_id)
	{
		PWSTR folder_path = nullptr;
		if (HRESULT result = SHGetKnownFolderPath(*folder_id, 0, nullptr, &folder_path); FAILED(result))
		{
			throw std::runtime_error("Failed to get folder path.");
		}
		fs::path path(folder_path);
		CoTaskMemFree(folder_path);
		return path;
	}
};