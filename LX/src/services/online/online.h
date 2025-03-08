#pragma once
#include "services/players/player_manager.hpp"

namespace services
{
	CNetGamePlayer* get_local_player();
	bool is_online();
	CNetGamePlayer* get_host();
	CNetGamePlayer* get_script_host();
	CNetGamePlayer** get_players();
	uint16_t get_player_limit();
	uint16_t get_player_count();

	FriendInfo* get_friend(uint32_t index);
	uint32_t get_friends_count();
	bool is_friend(CNetGamePlayer* net_player);

	void open_profile_by_rid(uint64_t rid);
	void give_collectible(Player target, eCollectibleType col, int index = 1, bool uncomplete = true);
}
