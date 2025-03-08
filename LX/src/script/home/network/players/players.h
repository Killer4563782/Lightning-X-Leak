#pragma once
#include "gui/types/submenu.hpp"
#include "services/players/player_manager.hpp"

namespace ui::submenus
{
	class player_submenu final : public submenu
	{
	public:
		player_ptr m_player;

		explicit player_submenu(player_ptr player, std::string description, submenu* parent = nullptr);

		void draw(bool selected) override;
		void on_enter() override;
		void on_init() override;
		void on_tick() override;
	};


	class players_submenu final : public submenu
	{
		int m_sort_index = 0;
		inline static std::vector<std::string> m_sorted_by{ "Name A-Z", "Index", "Rank" };

		using submenu::submenu;
		
		std::vector<player_ptr> get_sorted_players(CNetGamePlayer* left_player = nullptr) const;
		void update_players_list(bool skip_player_info = false);
		void on_tick() override;

	public:
		void on_player_join(CNetGamePlayer* player);
		void on_player_leave(CNetGamePlayer* player);
	};

	extern players_submenu g_players_submenu;
}
