#include "curl_client.h"

#include "curl/curl.h"
#include "thread/threadpool.h"

static size_t write_callback(const void* contents, size_t size, size_t nmemb, std::string* buffer)
{
    const size_t total_size = size * nmemb;
    buffer->append(static_cast<const char*>(contents), total_size);
    return total_size;
}

namespace curl_client
{
	void get(const std::string& link, const std::function<void(bool succeeded, nlohmann::json response)>& callback)
	{
		g_thread_pool->push([link, callback]
		{
			CURL* curl = curl_easy_init();
			if (!curl)
			{
				callback(false, {});
				return;
			}

			std::string read_buffer;

			curl_easy_setopt(curl, CURLOPT_URL, link.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);
			curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

			if (const CURLcode res = curl_easy_perform(curl); res != CURLE_OK)
			{
				LOG_DEBUG("Failed to perform request for: {} | Error: {}", link, curl_easy_strerror(res));
				callback(false, {});
				curl_easy_cleanup(curl);
				return;
			}

			try
			{
				nlohmann::json response = nlohmann::json::parse(read_buffer);
				callback(true, response);
			}
			catch (...)
			{
				LOG_DEBUG("Failed to parse response from: {}", link);
				callback(false, {});
			}

			curl_easy_cleanup(curl);
		});
	}

	void get_header(const std::string& link, const std::vector<std::pair<std::string, std::string>>& headers,
	                const std::function<void(bool succeeded, nlohmann::json response)>& callback)
	{
		g_thread_pool->push([link, headers, callback]
		{
			CURL* curl = curl_easy_init();
			if (!curl)
			{
				callback(false, {});
				return;
			}

			std::string read_buffer;
			curl_slist* header_list = nullptr;

			for (const auto& header : headers)
			{
				std::string header_string = header.first + ": " + header.second;
				header_list = curl_slist_append(header_list, header_string.c_str());
			}

			curl_easy_setopt(curl, CURLOPT_URL, link.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
			curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

			if (const CURLcode res = curl_easy_perform(curl); res != CURLE_OK)
			{
				LOG_DEBUG("Failed to perform request for: {} | Error: {}", link, curl_easy_strerror(res));
				callback(false, {});
				curl_easy_cleanup(curl);
				curl_slist_free_all(header_list);
				return;
			}

			try
			{
				nlohmann::json response = nlohmann::json::parse(read_buffer);
				callback(true, response);
			}
			catch (...)
			{
				LOG_DEBUG("Failed to parse response from: {}", link);
				callback(false, {});
			}

			curl_easy_cleanup(curl);
			curl_slist_free_all(header_list);
		});
	}

	void post(const std::string& link, const nlohmann::json& json, const std::function<void(bool succeeded, nlohmann::json response)>& callback)
	{
		g_thread_pool->push([link, json, callback]
		{
			CURL* curl = curl_easy_init();
			if (!curl)
			{
				callback(false, {});
				curl_easy_cleanup(curl);
				return;
			}

			std::string read_buffer;

			curl_easy_setopt(curl, CURLOPT_URL, link.c_str());
			curl_easy_setopt(curl, CURLOPT_POST, TRUE);
			//curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, json.length());
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, std::string(json.dump()).c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);
			curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

			if (const CURLcode res = curl_easy_perform(curl); res != CURLE_OK)
			{
				LOG_DEBUG("Failed to perform request for: {} | Error: {}", link, curl_easy_strerror(res));
				callback(false, {});
				curl_easy_cleanup(curl);
				return;
			}

			try
			{
				nlohmann::json response = nlohmann::json::parse(read_buffer);
				callback(true, response);
			}
			catch (...)
			{
				LOG_DEBUG("Failed to parse response from: {}", link);
				callback(false, {});
			}

			curl_easy_cleanup(curl);
		});
	}

	std::string parseJson(const std::string& json)
	{
		size_t startPos = json.find("[[[\"");
		if (startPos == std::string::npos)
		{
			return ""; 
		}
		size_t endPos = json.find("\"]]", startPos);
		if (endPos == std::string::npos)
		{
			return "";
		}
		size_t valueStart = json.find("[[\"", startPos);
		if (valueStart == std::string::npos || valueStart > endPos)
		{
			return ""; 
		}
		size_t valueEnd = json.find("\",", valueStart);
		if (valueEnd == std::string::npos || valueEnd > endPos)
		{
			return ""; 
		}
		return json.substr(valueStart + 3, valueEnd - valueStart - 3);
	}

	std::string translateText(const std::string& text, const std::string& targetLanguage)
	{
		using json = nlohmann::json;
		std::string translation;

		CURL* curl = curl_easy_init();
		if (curl)
		{
			std::string encodedText;
			for (char c : text)
			{
				if (c == ' ')
				{
					encodedText += "%20";
				}
				else
				{
					encodedText += c;
				}
			}

			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &translation);

			CURLcode res = curl_easy_perform(curl);
			if (res != CURLE_OK)
			{
				LOG(info, "Failed to perform HTTP request: {}", curl_easy_strerror(res)); 
			}

			curl_easy_cleanup(curl);
		}
		else
		{
			LOG(info, "Failed to initialize CURL"); 
		}

		return parseJson(translation);
	}
}