#include "api_service.hpp"

#include <curl/curl.h>

#include "http_request.h"
#include "fiber/fiber_pool.hpp"
#include "fiber/script.hpp"
#include "helpers/sessionStarterHelpers.h"
#include "rage/util.h"

uint64_t api_service::get_rid_from_username(std::string username)
{
	// Using Curl Method
    uint64_t rid = 0;

    // Initialize CURL
    CURL* curl = curl_easy_init();
    if (!curl)
        return 0;

    // Prepare the URL
    std::string url = "https://scui.rockstargames.com/api/friend/accountsearch";
    std::string post_data = R"({"searchNickname":")" + username + R"("})";

    // Set up headers
    curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "X-Requested-With: XMLHttpRequest");
    headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");

    // Add authorization header if sc_ticket is available
    if (auto sc_info = g_pointers->g_sc_info)
    {
        if (auto sc_ticket = sc_info->m_ticket)
        {
            std::string auth_header = "Authorization: SCAUTH val=\"" + std::string(sc_ticket) + "\"";
            headers = curl_slist_append(headers, auth_header.c_str());
        }
    }

    // Set up CURL options
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Response handling
    std::string response_data;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](void* ptr, size_t size, size_t nmemb, std::string* data) -> size_t
    {
        data->append(static_cast<char*>(ptr), size * nmemb);
        return size * nmemb;
    });
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);
    if (res == CURLE_OK)
    {
        // Parse the response
        try
        {
            nlohmann::json parsed_body = nlohmann::json::parse(response_data);
            if (parsed_body["Status"].get<bool>() && parsed_body["Total"].get<uint64_t>() > 0)
            {
                rid = parsed_body["Accounts"][0]["RockstarId"].get<uint64_t>();
            }
        }
        catch (const std::exception& e)
        {
            // Handle JSON parsing errors
			std::cerr << "JSON parsing error: " << e.what() << std::endl;
        }
    }
    else
    {
        // Handle CURL errors
        std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
    }

    // Clean up
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return rid;
}

std::string api_service::get_username_from_rid(uint64_t rid)
{
    // Initialize CURL
    CURL* curl = curl_easy_init();
    if (!curl)
        return "null";

    // Prepare the URL and POST data
    std::string url = "https://scui.rockstargames.com/api/friend/getprofile";
    std::string post_data = R"({"RockstarId":")" + std::to_string(rid) + R"("})";

    // Set up headers
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "X-Requested-With: XMLHttpRequest");
    headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");

    // Add authorization header if sc_ticket is available
    auto sc_info = g_pointers->g_sc_info;
    if (!sc_info)
    {
        curl_easy_cleanup(curl);
        return "null";
    }

    auto sc_ticket = sc_info->m_ticket;
    if (!sc_ticket)
    {
        curl_easy_cleanup(curl);
        return "null";
    }

    std::string auth_header = "Authorization: SCAUTH val=\"" + std::string(sc_ticket) + "\"";
    headers = curl_slist_append(headers, auth_header.c_str());

    // Set up CURL options
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Response handling
    std::string response_data;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](void* ptr, size_t size, size_t nmemb, std::string* data) -> size_t
    {
        data->append(static_cast<char*>(ptr), size * nmemb);
        return size * nmemb;
    });
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);
    if (res == CURLE_OK)
    {
        // Parse the response
        try
        {
            nlohmann::json parsed_body = nlohmann::json::parse(response_data);
            if (parsed_body["Status"].get<bool>())
            {
                for (auto& acc : parsed_body["Accounts"])
                {
                    if (rid == acc["RockstarAccount"]["RockstarId"].get<uint64_t>())
                    {
                        // Clean up and return the profile name
                        curl_slist_free_all(headers);
                        curl_easy_cleanup(curl);
                        return acc["RockstarAccount"]["Name"].get<std::string>();
                    }
                }
            }
        }
        catch (const std::exception& e)
        {
            // Handle JSON parsing errors
            std::cerr << "JSON parsing error: " << e.what() << std::endl;
        }
    }
    else
    {
        // Handle CURL errors
        std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
    }

    // Clean up
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return "null";
}

bool api_service::send_socialclub_message(uint64_t rid, std::string_view message)
{
	http::Request request("https://scui.rockstargames.com/api/messaging/sendmessage");
	nlohmann::json body = 
    {
        {
            std::format(R"({{"env":"prod","title":"gta5","version":11,"recipientRockstarId":"{}","messageText":"{}"}})", rid, message)
        }
	};

	if (auto sc_info = g_pointers->g_sc_info; auto scTicket = sc_info->m_ticket)
	{
		auto response = request.send("POST", body.dump(), { 
            { "X-Requested-With", "XMLHttpRequest" }, 
            { "Content-Type", "application/json; charset=utf-8" }, 
            { "Authorization", "SCAUTH val=\"" + std::string(scTicket) + "\""} 
        });

		auto parsed_body = nlohmann::json::parse(response.body.begin(), response.body.end());
		return http::Status::Ok;
	}
}

bool api_service::is_rid_online(uint64_t rid)
{
    // Initialize CURL
    CURL* curl = curl_easy_init();
    if (!curl)
        return false;

    // Prepare the URL and POST data
    std::string url = "https://scui.rockstargames.com/api/friend/getprofile";
    std::string post_data = R"({"RockstarId":")" + std::to_string(rid) + R"("})";

    // Set up headers
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "X-Requested-With: XMLHttpRequest");
    headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");

    // Add authorization header if sc_ticket is available
    auto sc_info = g_pointers->g_sc_info;
    if (!sc_info)
    {
        curl_easy_cleanup(curl);
        return false;
    }

    auto sc_ticket = sc_info->m_ticket;
    if (!sc_ticket)
    {
        curl_easy_cleanup(curl);
        return false;
    }

    std::string auth_header = "Authorization: SCAUTH val=\"" + std::string(sc_ticket) + "\"";
    headers = curl_slist_append(headers, auth_header.c_str());

    // Set up CURL options
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Response handling
    std::string response_data;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](void* ptr, size_t size, size_t nmemb, std::string* data) -> size_t
    {
        data->append(static_cast<char*>(ptr), size * nmemb);
        return size * nmemb;
    });
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);
    if (res == CURLE_OK)
    {
        // Parse the response
        try
        {
            nlohmann::json parsed_body = nlohmann::json::parse(response_data);
            if (parsed_body["Status"].get<bool>())
            {
                for (const auto& acc : parsed_body["Accounts"])
                {
                    const auto& acc_data = acc["RockstarAccount"];
                    if (rid == acc_data["RockstarId"].get<uint64_t>())
                    {
                        // Clean up and return the online status
                        curl_slist_free_all(headers);
                        curl_easy_cleanup(curl);
                        return acc_data["IsOnline"].get<bool>();
                    }
                }
            }
        }
        catch (const std::exception& e)
        {
            // Handle JSON parsing errors
            std::cerr << "JSON parsing error: " << e.what() << std::endl;
        }
    }
    else
    {
        // Handle CURL errors
        std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
    }

    // Clean up
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return false;
}

void api_service::join_session(const rage::rlSessionInfo& info)
{
	if (SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("maintransition"_joaat) != 0 || STREAMING::IS_PLAYER_SWITCH_IN_PROGRESS())
	{
		return notify(Error, 3000, "Player switch in progress, Please wait.");
	}

	g_api_service->join_queued = true;
	g_api_service->queued_info = info;
	session_starter::join_type(eSessionType::NEW_PUBLIC);
	script::get_current()->yield(350ms);
	if (const auto running_threads = SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("maintransition"_joaat); running_threads == 0)
	{
		g_api_service->join_queued = false;
		notify(Error, 3000, "Failed to launch transition to join player");
	}
}

void api_service::rid_join(uint64_t rid)
{
	g_fiber_pool->queue_job([&]
	{
		switch (g_api_service->rid_joiner_idx)
		{
			case 0:
			{
				if (SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("maintransition"_joaat) != 0 || STREAMING::IS_PLAYER_SWITCH_IN_PROGRESS())
				{
					return notify(Error, 5s, "Player switch in progress, Please wait.");
				}

				rage::rlGamerHandle player_handle(rid);
				rage::rlSessionByGamerTaskResult result;
				bool success = false;
				rage::rlTaskStatus state{};

				if (g_pointers->g_start_get_session_by_gamer_handle(0, &player_handle, 1, &result, 1, &success, &state))
				{
					while (state.status == 1)
						script::get_current()->yield();

					if (state.status == 3 && success)
					{
						join_session(result.m_session_info);
					}
				}
				else
				{
					if (is_rid_online(rid))
					{
						notify(Error, 5s, "Failed to join due to unknown reason");
					}
					else
					{
						notify(Error, 5s, "Player is offline");
					}
				}

				break;
			}
			case 1: // Seems to work only in online
			{
				if (SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("maintransition"_joaat) != 0 || STREAMING::IS_PLAYER_SWITCH_IN_PROGRESS())
				{
					return notify(Error, 3000, "Player switch in progress, Please wait.");
				}
				auto player_handle = rage::rlGamerHandle(rid);
				auto success = false;
				rage::rlTaskStatus state{};
				if (rage::rlSessionByGamerTaskResult task_res; g_pointers->g_start_get_session_by_gamer_handle(0, &player_handle, 1, &task_res, 1, &success, &state))
				{
					while (state.status == 1)
						script::get_current()->yield();

					if (state.status == 3 && success)
					{
						rage::rlPresenceEventInviteAccepted inv(get_local_playerinfo()->m_net_player_data, task_res.m_session_info, task_res.m_gamer_handle);
						g_pointers->g_send_invite_accepted_presence_event(g_pointers->g_presecne_struct, &inv, 0x21);
						return;
					}
				}

				if (is_rid_online(rid))
				{
					notify(Error, 5s, "Failed to join due to unknown reason");
				}
				else
				{
					notify(Error, 5s, "Player is offline");
				}
				break;
			}
			default:;
		}
	});
}

void api_service::rid_invite(uint64_t rid)
{
	g_fiber_pool->queue_job([&]
	{
		rage::rlGamerHandle player_handle(rid);

		bool success = g_pointers->g_invite_player_by_gamer_handle(*g_pointers->g_network, &player_handle, 1, nullptr, nullptr, nullptr);

		if (!success)
		{
			if (is_rid_online(rid))
			{
				notify(Error, 5s, "Failed to send game invite due to unknown reason");
			}
			else
			{
				notify(Error, 5s, "Player is offline");
			}
			return;
		}

		notify(Success, "Successfully sent game invite!");
	});
}
