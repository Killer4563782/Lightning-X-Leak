#include "playerInfo.h"

#include "gxtLabelHelpers.h"
#include "networkHelpers.h"
#include "core/curl_client/curl_client.h"
#include "fonts/icons.h"
#include "gui/drawing.h"
#include "gui/gui.h"
#include "json/json.h"
#include "services/online/online.h"
#include "util/globals.h"
#include "util/util.h"

namespace player_info
{
	std::string get_player_tags(const player_ptr& player)
	{
		if (!player || !player->is_valid())
			return "";

		std::vector<std::string> tags;
		tags.reserve(12); // Adjusted based on the number of possible tags

		if (player->m_is_friend)
			tags.emplace_back("F");

		const auto& net_player = player->m_net_game_player;
		
		if (player->m_is_modder || net_player->m_is_cheater || net_player->m_is_rockstar_dev || net_player->m_is_rockstar_qa)
			tags.emplace_back("M");

		if (player->is_host())
			tags.emplace_back("H");

		if (services::get_script_host() == net_player)
			tags.emplace_back("SH");

		if (net_player->m_is_spectating)
			tags.emplace_back("S");

		if (network_helpers::is_in_godmode(player) && !network_helpers::is_in_interior(player))
			tags.emplace_back("G");

		if (auto vehicle = player->get_current_vehicle(); vehicle && (vehicle->m_damage_bits & God))
			tags.emplace_back("VG");

		if (network_helpers::is_invisible(player) && !network_helpers::is_in_cutscene(player))
			tags.emplace_back("V");

		if (network_helpers::is_hidden_from_player_list(player))
			tags.emplace_back("HP");

		if (auto ped = player->get_ped(); ped && ped->m_health <= 0.f)
			tags.emplace_back("D");

		if (network_helpers::is_in_interior(player))
			tags.emplace_back("I");

		if (network_helpers::is_in_cutscene(player))
			tags.emplace_back("C");

		if (tags.empty())
			return "";

		std::ostringstream oss;
		oss << "[";
		for (size_t i = 0; i < tags.size(); ++i)
		{
			oss << tags[i];
			if (i < tags.size() - 1)
				oss << ", ";
		}
		oss << "]";

		return oss.str();
	}

	void draw_title(const std::string& title, const ImVec2& pos, const ImVec2& size)
	{
		drawing::rectangle(pos, { size.x, 0.04f }, g_background_color);
		drawing::text(
			g_options.m_font,
			title,
			{ pos.x + size.x / 2.f, pos.y + 0.01f },
			0.32f,
			menu_color_white,
			eJustify::Center,
			g_options.m_padding,
			size.x
		);
	}
	void draw_single_line(const std::string& icon, const std::string& text, const std::string& data, const ImVec2& pos,
		const ImVec2& size)
	{
		drawing::rectangle(pos, { size.x, 0.035f }, g_background_color);
		drawing::text(
			FontAwesome,
			icon,
			{ pos.x, pos.y + drawing::get_text_height(FontAwesome, 0.26f) / 2.f },
			0.26f,
			menu_color_white,
			eJustify::Left,
			g_options.m_padding,
			size.x
		);
		drawing::text(
			g_options.m_font,
			text,
			{ pos.x + 0.01f, pos.y + 0.01f },
			0.26f,
			menu_color_white,
			eJustify::Left,
			g_options.m_padding,
			size.x
		);
		drawing::text(
			g_options.m_font,
			data,
			{ pos.x + size.x - 0.01f, pos.y + 0.01f },
			0.26f,
			menu_color_white,
			eJustify::Right,
			g_options.m_padding,
			size.x
		);
	}
	void draw_single_line_number(const int& number, const std::string& text, const std::string& data, const ImVec2& pos, const ImVec2& size)
	{
		drawing::rectangle(pos, { size.x, 0.035f }, g_background_color);
		drawing::text(
			g_options.m_font,
			std::to_string(number),
			{ number < 10 ? pos.x : pos.x, pos.y + drawing::get_text_height(g_options.m_font, 0.26f) / 2.f },
			0.26f,
			menu_color_white,
			eJustify::Left,
			g_options.m_padding,
			size.x
		);
		drawing::text(
			g_options.m_font,
			text,
			{ pos.x + 0.01f, pos.y + 0.01f },
			0.26f,
			menu_color_white,
			eJustify::Left,
			g_options.m_padding,
			size.x
		);
		drawing::text(
			g_options.m_font,
			data,
			{ pos.x + size.x - 0.01f, pos.y + 0.01f },
			0.26f,
			menu_color_white,
			eJustify::Right,
			g_options.m_padding,
			size.x
		);
	}
	void draw_double_line(const std::string& icon, const std::string& text1, const std::string& data1,
		const std::string& icon2, const std::string& text2,
		const std::string& data2, const ImVec2& pos, const ImVec2& size)
	{
		drawing::rectangle(pos, { size.x, 0.035f }, g_background_color);

		drawing::text(
			FontAwesome,
			icon,
			{ pos.x, pos.y + drawing::get_text_height(FontAwesome, 0.26f) / 2.f },
			0.26f,
			menu_color_white,
			eJustify::Left,
			g_options.m_padding,
			size.x
		);

		drawing::text(
			g_options.m_font,
			text1,
			{ pos.x + 0.01f, pos.y + 0.01f },
			0.26f,
			menu_color_white,
			eJustify::Left,
			g_options.m_padding,
			size.x
		);

		drawing::text(
			g_options.m_font,
			data1,
			{ pos.x + size.x / 2.f - 0.01f, pos.y + 0.01f },
			0.26f,
			menu_color_white,
			eJustify::Right,
			g_options.m_padding,
			size.x
		);

		drawing::rectangle({ pos.x + size.x / 2.f, pos.y }, { 0.0015f, 0.035f }, menu_grey_lines);

		drawing::text(
			FontAwesome,
			icon2,
			{ pos.x + size.x / 2.f, pos.y + drawing::get_text_height(FontAwesome, 0.26f) / 2.f },
			0.26f,
			menu_color_white,
			eJustify::Left,
			g_options.m_padding,
			size.x);

		drawing::text(
			g_options.m_font,
			text2,
			{ pos.x + size.x / 2.f + 0.01f, pos.y + 0.01f },
			0.26f,
			menu_color_white,
			eJustify::Left,
			g_options.m_padding,
			size.x
		);

		drawing::text(
			g_options.m_font,
			data2,
			{ pos.x + size.x - 0.01f, pos.y + 0.01f },
			0.26f,
			menu_color_white,
			eJustify::Right,
			g_options.m_padding,
			size.x
		);
	}

	size_t write_callback(const void* contents, size_t size, size_t nmemb, std::string* buffer)
	{
		const size_t total_size = size * nmemb;
		buffer->append(static_cast<const char*>(contents), total_size);
		return total_size;
	}

	geo_location_status get_geo_location(const std::string& ip, geo_location* out)
	{
		if (geo_location_cache.contains(ip))
		{
			const auto geo_loc = geo_location_cache[ip];
			if (geo_loc.status != Success)
				return geo_loc.status;

			*out = geo_loc;
			return Success;
		}

		return NotReady;
	}

	std::string format_with_commas(const int value)
	{
		std::ostringstream oss;
		oss.imbue(std::locale(""));
		oss << std::fixed << value;
		return oss.str();
	}

	void player_info(const player_ptr& player)
	{
		const auto rl_gamer_info = player->get_player_info();

		if (!g_open || !rl_gamer_info || !player->is_valid())
			return;

		auto name = player->get_name();
		auto ped = player->get_ped();
		auto vehicle = player->get_current_vehicle();
		auto pos = ped->get_position();

		if (vehicle && vehicle->m_model_info)
		{
			player_vehicle_god = std::format("{} (Health: {}/{})", vehicle->m_damage_bits & God ? "Yes" : "No", std::round(vehicle->m_health), std::round(vehicle->m_maxhealth));

			auto model_info = reinterpret_cast<CVehicleModelInfo*>(vehicle->m_model_info);
			vehicle_label = gxt::get_gxt_label(model_info->m_name).value_or("INVALID");
			if (auto manufacturer_label = gxt::get_gxt_label(model_info->m_manufacturer); manufacturer_label.value_or("NULL") != "NULL")
				vehicle_label += std::string(" ") + "(" + manufacturer_label.value() + ")";
		}

		auto& stats = scr_globals::gpbd_fm_1.as<GPBD_FM*>()->Entries[player->id()].PlayerStats;
		const auto& boss_goon = scr_globals::gpbd_fm_3.as<GPBD_FM_3*>()->Entries[player->id()].BossGoon;
		auto ip = player->get_ip_address();
		const auto port = player->get_port();

		ImVec2 start_pos = { g_pos.x + g_width + 0.005f, g_pos.y };
		constexpr ImVec2 size = { 0.225f, 0.6f };
		bool is_menu_on_the_left_side = g_pos.x < 0.45f;
		start_pos.x = is_menu_on_the_left_side ? g_pos.x + size.x + 0.01f : g_pos.x - size.x - 0.005f;
		auto ped_model_hash = static_cast<rage::joaat_t>(ped->m_model_info->m_hash);

		// Player Info
		draw_title(name, start_pos, size);
		if (ped)
		{
			draw_single_line(ICON_FA_USER_SECRET, "GodMode:", ped->m_damage_bits & God ? "Yes" : "No", { start_pos.x, start_pos.y + 0.04f }, size);
			draw_single_line(ICON_FA_HEART, "Health:", std::format("{}/{} ({}%)", ped->m_health, ped->m_maxhealth, std::round(ped->m_health / ped->m_maxhealth * 100.f)), { start_pos.x, start_pos.y + 0.075f }, size);
			draw_single_line(ICON_FA_SHIELD, "Armor:", std::format("{}/{} ({}%)", ped->m_armor, ped->m_player_info->m_max_armor, std::round(ped->m_armor / static_cast<float>(ped->m_player_info->m_max_armor) * 100.f)), { start_pos.x, start_pos.y + 0.11f }, size);
			draw_single_line(ICON_FA_STAR, "Wanted Level:", std::to_string(rl_gamer_info->m_wanted_level), { start_pos.x, start_pos.y + 0.145f }, size);
			draw_single_line(ICON_FA_LOCATION_PIN, "Position:", std::format("{:.2f}, {:.2f}, {:.2f}", pos->x, pos->y, pos->z), { start_pos.x, start_pos.y + 0.18f }, size);
			draw_single_line(ICON_FA_CAR, "Vehicle:", vehicle ? vehicle_label.c_str() : "N/A", { start_pos.x, start_pos.y + 0.215f }, size);
			draw_single_line(ICON_FA_CAR_BURST, "Vehicle GodMode:", vehicle ? player_vehicle_god : "N/A", { start_pos.x, start_pos.y + 0.25f }, size);
			draw_double_line(ICON_FA_HOUSE, "In Interior:", network_helpers::is_in_interior(player) ? "Yes" : "No", ICON_FA_CAMERA, "In CutScene", network_helpers::is_in_cutscene(player) ? "Yes" : "No", { start_pos.x, start_pos.y + 0.285f }, size);
			draw_double_line(ICON_FA_GUN, "Kills:", std::format("{}", format_with_commas(stats.KillsOnPlayers)), ICON_FA_CROSSHAIRS, "Accuracy:", std::format("{:.2f}%", stats.WeaponAccuracy), { start_pos.x, start_pos.y + 0.32f }, size);
			draw_double_line(ICON_FA_SKULL, "Deaths:", std::format("{}", format_with_commas(stats.DeathsByPlayers)), ICON_FA_CHART_PIE, "KD Ratio:", std::format("{:.2f}", stats.KdRatio), { start_pos.x, start_pos.y + 0.355f }, size);
			draw_double_line(ICON_FA_GLOBE, "Rank:", std::to_string(stats.Rank), ICON_FA_LAYER_GROUP, "RP:", format_with_commas(stats.RP), { start_pos.x, start_pos.y + 0.39f }, size);
			draw_double_line(ICON_FA_WALLET, "Wallet:", std::format("${}", format_with_commas(stats.WalletBalance)), ICON_FA_BUILDING_COLUMNS, "Bank:", std::format("${}", format_with_commas(stats.Money - stats.WalletBalance)), { start_pos.x, start_pos.y + 0.425f }, size);
			draw_single_line(ICON_FA_MONEY_BILLS, "Total Money:", std::format("${}", format_with_commas(stats.Money)), { start_pos.x, start_pos.y + 0.46f }, size);
			if (ped && ped->m_model_info && ped_model_hash)
			{
				draw_single_line(ICON_FA_PERSON, "Player Model:", std::format("{} (0x{:X})", ped_model_hash == "mp_f_freemode_01"_joaat ? "Female" : "Male", ped_model_hash), {start_pos.x, start_pos.y + 0.495f}, size);
			}
			else
			{
				draw_single_line(ICON_FA_PERSON, "Player Model:", std::format("Invalid (0x{:X})", ped_model_hash), { start_pos.x, start_pos.y + 0.495f }, size);
			}
			draw_single_line(ICON_FA_RECEIPT, "Host Token:", std::format("0x{:X}", rl_gamer_info->m_net_player_data.m_host_token), { start_pos.x, start_pos.y + 0.53f }, size);
			int lang_index = boss_goon.Language;
			draw_single_line(ICON_FA_LANGUAGE, "Language:", lang_index >= 0 && lang_index < 13 ? languages[lang_index].name : "Invalid", { start_pos.x, start_pos.y + 0.565f }, size);
			draw_double_line(ICON_FA_PASSPORT, "RID:", std::to_string(rl_gamer_info->m_net_player_data.m_gamer_handle.m_rockstar_id), ICON_FA_CHAIR, "PID/Slot:", std::to_string(player->id()), { start_pos.x, start_pos.y + 0.6f }, size);
			drawing::rectangle_not_filled(start_pos, { size.x, 0.635f }, menu_grey_lines, 2.f);
		}
		else
		{
			draw_single_line(ICON_FA_ARROWS_SPIN, "Status:", "Player is loading", { start_pos.x, start_pos.y + 0.04f }, size);
			drawing::rectangle_not_filled(start_pos, { size.x, 0.075f }, menu_grey_lines, 2.f);
		}

		// Geo Info
		start_pos.x = is_menu_on_the_left_side ? start_pos.x + 0.23f : start_pos.x - 0.23f;
		//g_pointers->g_draw_sprite(texture, texture, start_pos.x + 0.05f, start_pos.y + 0.24f, 0.035f, 0.035f, 0.f, 255, 255, 255, 255, true, true);
		draw_title("Geo Location", start_pos, size);
		if (ip.has_value())
		{
			std::string ip_string = std::format("{}.{}.{}.{}", ip->m_field1, ip->m_field2, ip->m_field3, ip->m_field4);
			geo_location geo_loc;
			if (auto status = get_geo_location(ip_string, &geo_loc); status == Success)
			{
				draw_single_line(ICON_FA_FLAG, "Country:", geo_loc.country, { start_pos.x, start_pos.y + 0.04f }, size);
				draw_single_line(ICON_FA_MAP, "Region:", geo_loc.region, { start_pos.x, start_pos.y + 0.075f }, size);
				draw_single_line(ICON_FA_CITY, "City:", geo_loc.city, { start_pos.x, start_pos.y + 0.11f }, size);
				draw_single_line(ICON_FA_FILE_ZIPPER, "Postal:", geo_loc.postcode, { start_pos.x, start_pos.y + 0.145f }, size);
				draw_single_line(ICON_FA_NETWORK_WIRED, "ISP:", geo_loc.provider, { start_pos.x, start_pos.y + 0.18f }, size);
				draw_double_line(ICON_FA_WIFI, "IP:", ip_string, ICON_FA_SERVER, "Port:", std::to_string(port), { start_pos.x, start_pos.y + 0.215f }, size);
				draw_single_line(ICON_FA_WIFI, "VPN:", geo_loc.isVPN ? "Yes" : "No", { start_pos.x, start_pos.y + 0.25f }, size);
				if (geo_loc.isVPN)
				{
					draw_single_line(ICON_FA_WIFI, "VPN Provider:", geo_loc.vpnName, { start_pos.x, start_pos.y + 0.285f }, size);
					drawing::rectangle_not_filled(start_pos, { size.x, 0.32f }, menu_grey_lines, 2.f);

				}
				else drawing::rectangle_not_filled(start_pos, { size.x, 0.285f }, menu_grey_lines, 2.f);
			}
			else
			{
				draw_single_line(status == Failed ? ICON_FA_XMARK : ICON_FA_ARROWS_SPIN, "Status:", status == Failed ? "Failed" : "Loading...", { start_pos.x, start_pos.y + 0.04f }, size);
				drawing::rectangle_not_filled(start_pos, { size.x, 0.075f }, menu_grey_lines, 2.f);
			}
		}
		else
		{
			draw_single_line(ICON_FA_XMARK, "Status:", "Failed to get IP address", { start_pos.x, start_pos.y + 0.04f }, size);
			drawing::rectangle_not_filled(start_pos, { size.x, 0.075f }, menu_grey_lines, 2.f);
		}
	}

	void friend_info(const FriendInfo* friends)
	{
		if (!g_open || !friends)
			return;

		// Extract friend information
		const std::string& name = friends->m_name;
		const uint64_t rockstar_id = friends->m_rockstar_id;
		const bool is_joinable = friends->m_is_joinable;

		// Calculate the starting position for the menu
		constexpr ImVec2 size = { 0.225f, 0.2f };
		const bool is_menu_on_the_left_side = g_pos.x < 0.45f;
		const float menu_offset = is_menu_on_the_left_side ? size.x + 0.01f : -size.x - 0.005f;
		const ImVec2 start_pos = { g_pos.x + menu_offset, g_pos.y };

		// Draw the title and friend information
		draw_title(name, start_pos, size);
		draw_single_line(ICON_FA_ID_CARD, "Rockstar ID:", std::to_string(rockstar_id), { start_pos.x, start_pos.y + 0.04f }, size);
		draw_single_line(ICON_FA_USER, "Joinable:", is_joinable ? "Yes" : "No", { start_pos.x, start_pos.y + 0.075f }, size);

		// Draw a rectangle around the friend info
		constexpr float rectangle_height = 0.11f;
		drawing::rectangle_not_filled(start_pos, { size.x, rectangle_height }, menu_grey_lines, 2.f);
	}

	void session_host_queue_info()
	{
		if (!g_open || !services::is_online())
			return;

		constexpr ImVec2 size = { 0.225f, 0.6f };
		const auto is_menu_on_the_left_side = g_pos.x < 0.45f;
		const auto menu_offset = is_menu_on_the_left_side ? size.x + 0.01f : -size.x - 0.005f;
		ImVec2 start_pos = { g_pos.x + menu_offset, g_pos.y };
		auto vertical_offset = 0.04f;

		// Get the player manager and its player count
		auto* player_mgr = get_network_player_mgr();
		const auto player_count = player_mgr->m_player_count;

		// Collect players who are not the host
		std::vector<std::pair<CNetGamePlayer*, uint64_t>> host_queue;
		host_queue.reserve(player_count); // Reserve space to avoid reallocations

		for (CNetGamePlayer* player : player_mgr->m_player_list)
		{
			if (player && player->is_valid() && !player->is_host())
			{
				host_queue.emplace_back(player, player->get_net_data()->m_host_token);
			}
		}

		// If the host queue is empty, display a message and return
		if (host_queue.empty())
		{
			draw_single_line(ICON_FA_SERVER, "Host Queue is empty", "", { start_pos.x, start_pos.y + vertical_offset }, size);
			return;
		}

		// Sort the host queue by host token
		std::ranges::sort(host_queue, [](const auto& a, const auto& b) {
			return a.second < b.second;
		});

		// Draw the host queue title
		draw_title(std::format("Host Queue ({} Players)", player_count - 1), start_pos, size);

		// Draw the first 16 players in the host queue
		int queue_position = 1;
		for (const auto& [player, host_token] : host_queue | std::views::take(16))
		{
			draw_single_line_number(queue_position++, player->get_name(), std::format("0x{:X}", host_token), { start_pos.x, start_pos.y + vertical_offset }, size);
			vertical_offset += 0.035f;
		}

		// If there are more than 16 players, draw the remaining players in a new column
		if (host_queue.size() > 16)
		{
			const float column_offset = is_menu_on_the_left_side ? 0.23f : -0.23f;
			start_pos.x += column_offset;
			vertical_offset = 0.04f;

			draw_title("Host Queue (Continued)", start_pos, size);

			for (const auto& [player, host_token] : host_queue | std::views::drop(16))
			{
				draw_single_line_number(queue_position++, player->get_name(), std::format("0x{:X}", host_token), { start_pos.x, start_pos.y + vertical_offset }, size);
				vertical_offset += 0.035f;
			}
		}
	}
}
