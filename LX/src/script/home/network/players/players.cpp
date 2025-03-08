#include "players.h"

#include "commands/manager.hpp"
#include "gui/interface.h"
#include "gui/types/break_option.hpp"
#include "gui/types/function_option.hpp"
#include "gui/types/toggle_option.hpp"
#include "gui/types/vector_option.hpp"
#include "helpers/networkHelpers.h"
#include "helpers/playerInfo.h"
#include "player_kick/player_kick.h"
#include "player_teleport/player_teleport.h"
#include "player_trolling/player_trolling.h"
#include "player_crash/player_crash.h"
#include "player_friendly/friendly.h"
#include "script/home/network/network.h"
#include "services/online/online.h"

namespace ui::submenus
{
	player_submenu::player_submenu(player_ptr player, std::string description, submenu* parent) : submenu(player->get_name(), std::move(description), parent),
		m_player(std::move(player))
	{}

	color rank_color(const int rank)
	{
	    if (rank < 99)
	        return { 45, 110, 185, 255 };
	    if (rank <= 499)
		    return { 180, 130, 97, 255 };
	    if (rank <= 749)
		    return { 150, 153, 161, 255 };
	    return { 214, 181, 99, 255 };
	}

	void player_submenu::draw(bool selected)
	{
		const std::string name = std::string(m_player->get_name()) + " " + player_info::get_player_tags(m_player);

		drawing::text(
			g_options.m_font,
			name,
			{
				g_pos.x,
				g_base + g_options.m_size / 2.f - drawing::get_text_size(g_options.m_font, g_options.m_text_size, name).y / 2.f
			},
			g_options.m_text_size,
			g_options.text_color(selected),
			g_options.m_text_justify,
			g_options.m_padding,
			g_width
		);

		drawing::image(
			g_options.m_globe_sprite_bg,
			{
				g_pos.x + g_width - g_options.m_padding / drawing::g_resolution.x - 0.015f,
				g_base + g_options.m_size / 2.f
			},
			{ 1.2f, 1.2f },
			{ 0, 0, 0, 255 }
		);

		drawing::image(
			g_options.m_globe_sprite,
			{
				g_pos.x + g_width - g_options.m_padding / drawing::g_resolution.x - 0.015f,
				g_base + g_options.m_size / 2.f
			},
			{ 1.2f, 1.2f },
			rank_color(m_player->rank())
		);

		drawing::text(
			g_options.m_font,
			std::to_string(m_player->rank()),
			{
				g_pos.x + g_width - g_options.m_padding / drawing::g_resolution.x - 0.02f,
				g_base + g_options.m_size / 2.f - drawing::get_text_size(g_options.m_font, g_options.m_text_size, std::to_string(m_player->rank())).y / 2.f
			},
			g_options.m_text_size,
			{ 255, 255, 255, 255 },
			eJustify::Center,
			g_options.m_padding,
			g_width
		);

		drawing::text(
			g_options.m_toggle_font,
			"e",
			{
				g_pos.x + g_width + 0.003f,
				g_base + g_options.m_size / 2.f - drawing::get_text_height(g_options.m_toggle_font, g_options.m_text_size) / 2.f
			},
			g_options.m_text_size,
			g_options.text_color(selected),
			eJustify::Right,
			-g_options.m_padding,
			g_width
		);

		g_base += g_options.m_size;
	}

	void player_submenu::on_enter()
	{
		m_current = 0;
		m_scroll_position = 0;
		g_player_manager->set_selected(m_player);
	}

	void player_submenu::on_init()
	{
		this->add(toggle_option("spectate"_LC));
		this->add(player_teleport_submenu("Teleport"));
		this->add(player_friendly_submenu("Friendly"));
		this->add(player_trolling_submenu("Trolling"));
		this->add(player_kick_submenu("Kicks"));
		this->add(player_crash_submenu("Crash"));
		this->add(function_option("Open Profile", {}, [] { "open_player_profile"_PC->call(g_player_manager->get_selected(), {}); }));
		this->add(function_option("Send Friend Request", {}, [] { "send_friend_request"_PC->call(g_player_manager->get_selected(), {}); }));
	}

	void player_submenu::on_tick()
	{
		player_info::player_info(m_player);
	}

	std::vector<player_ptr> players_submenu::get_sorted_players(CNetGamePlayer* left_player) const
	{
		std::vector<player_ptr> sorted_players;
		sorted_players.reserve(32);

		for (const auto& player : g_player_manager->players() | std::views::values)
			if (player) sorted_players.emplace_back(player);

		if (auto self = g_player_manager->get_self())
			sorted_players.emplace_back(self);

		// Used for on_player_leave to include this player in the sorted player list so don't have to call g_player_manager->player_leave after wards
		if (left_player)
		{
			auto player = std::make_shared<services::player>(left_player);
			sorted_players.emplace_back(std::move(player));
		}

		switch (m_sort_index)
		{
		case 0:
			std::ranges::sort(sorted_players, [this](const player_ptr& a, const player_ptr& b)
			{
				return to_lower(trim_non_characters(a->get_name())) < to_lower(trim_non_characters(b->get_name()));
			});
			break;
		case 1:
			std::ranges::sort(sorted_players, [this](const player_ptr& a, const player_ptr& b)
			{
				return a->id() < b->id();
			});
			break;
		case 2:
			std::ranges::sort(sorted_players, [this](const player_ptr& a, const player_ptr& b)
			{
				return a->rank() < b->rank();
			});
			break;
		}

		return sorted_players;
	}

	void players_submenu::update_players_list(bool skip_player_info)
	{
		m_options.clear();

		this->add(vector_option("Sort by", {}, &m_sort_index, &m_sorted_by));
		this->add(break_option(std::to_string(services::get_player_count()) + " Players"));
		if (!services::is_online())
			return;

		const std::vector<player_ptr> sorted_players = get_sorted_players();

		// - 2 because of Sort By vector_option and Players break_option
		if (!skip_player_info && m_current >= 2 && sorted_players.size() > m_current - 2)
			player_info::player_info(sorted_players[m_current - 2]);

		for (const auto& player : sorted_players)
		{
			this->add(player_submenu(player, {}));
		}
	}

	std::mutex g_player_list_mutex;
	void players_submenu::on_tick()
	{
		std::lock_guard lock(g_player_list_mutex);
		g_player_manager->set_selected(g_player_manager->m_dummy);
		update_players_list();
	}

	void players_submenu::on_player_join(CNetGamePlayer* player)
	{
		if (m_current < 2)
			return;

		std::lock_guard lock(g_player_list_mutex);

		auto sorted_players = get_sorted_players();
		const auto it = std::ranges::find_if(sorted_players, [player](const player_ptr& p)
		{
			return p->m_net_game_player && p->m_net_game_player->m_active_id == player->m_active_id;
		});
		if (it == sorted_players.end())
			return;

		if (const auto index = it - sorted_players.begin(); index >= 0 && static_cast<uint64_t>(index) <= m_current - 2 && m_current < sorted_players.size() - 1 + 2)
		{
			update_players_list();
			action(ActionType::Down);
		}
	}

	void players_submenu::on_player_leave(CNetGamePlayer* player)
	{
		if (g_player_manager->m_selected_player->m_net_game_player && player->m_active_id == g_player_manager->m_selected_player->m_net_game_player->m_active_id)
		{
			g_player_manager->m_selected_player = g_player_manager->m_dummy;

			// If current menu is a player submenu then go back to all players submenu
			const submenu* menu = g_current_menu;
			while (menu->m_parent)
			{
				menu = menu->m_parent;
				if (menu == &g_players_submenu)
				{
					g_current_menu = &g_players_submenu;
					break;
				}
			}
		}

		if (m_current < 2)
			return;

		std::lock_guard lock(g_player_list_mutex);

		auto sorted_players = get_sorted_players(player);
		const auto it = std::ranges::find_if(sorted_players, [player](const player_ptr& p)
		{
			return p->m_net_game_player->m_active_id == player->m_active_id;
		});
		if (it == sorted_players.end())
			return;

		if (const auto index = it - sorted_players.begin(); index >= 0 && static_cast<uint64_t>(index) <= m_current - 2)
		{
			update_players_list();
			action(ActionType::Up);
		}
	}

	players_submenu g_players_submenu("Players", {}, &g_network_submenu);
}
