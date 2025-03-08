#pragma once
#include "json/json.h"

namespace curl_client
{
	void get(const std::string& link, const std::function<void(bool succeeded, nlohmann::json response)>& callback);
	void get_header(const std::string& link, const std::vector<std::pair<std::string, std::string>>& headers, const std::function<void(bool succeeded, nlohmann::json response)>& callback);
	void post(const std::string& link, const std::string& json, const std::function<void(bool succeeded, nlohmann::json)>& callback);
	std::string translateText(const std::string& text, const std::string& targetLang);
}