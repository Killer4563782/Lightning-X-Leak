#pragma once
#include "services/players/player_manager.hpp"

namespace network_helpers
{
	Player get_interior_from_player(Player player);
	bool is_in_cutscene(const player_ptr& player);
	bool is_in_interior(const player_ptr& player);
	bool is_in_godmode(const player_ptr& player);
	bool is_invisible(const player_ptr& player);
	bool is_hidden_from_player_list(const player_ptr& player);
	bool is_using_rc_vehicle(const player_ptr& player);
	bool is_using_orbital_cannon(const player_ptr& player);
	void open_players_profile(const player_ptr& player);
	void set_player_model(rage::joaat_t model);
}

namespace mechanic
{
	Vehicle get_personal_vehicle();
}

namespace crashes
{
	rage::fwArchetype* get_model_info(const rage::joaat_t hash); 
	void ped_crash(const player_ptr& player); 

	//We need to name them 
	void crash_1(const player_ptr& player); 
	void crash_2(const player_ptr& player); 
	void crash_3(const player_ptr& player); 
	void crash_4(const player_ptr& player); 
	void crash_5(const player_ptr& player); 
	void crash_6(const player_ptr& player); 
	void crash_7(const player_ptr& player); 
	void crash_8(const player_ptr& player); 
	void crash_9(const player_ptr& player); 
}