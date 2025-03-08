#include "fiber/fiber_pool.hpp"
#include "hooking/hooking.hpp"
#include "script/home/network/players/players.h"
#include "services/players/player_manager.hpp"

inline bool is_spoofed_host_token(rage::rlGamerInfo* info)
{
	if (info->m_host_token < INT_MAX)
		return true;

	return info->m_peer_id >> 32 != info->m_host_token >> 32;
}

void* hooks::assign_physical_index(CNetworkPlayerMgr* netPlayerMgr, CNetGamePlayer* player, uint8_t new_index)
{
	const auto* net_player_data = player->get_net_data();

	if (new_index == static_cast<uint8_t>(-1))
	{
		submenus::g_players_submenu.on_player_leave(player);
		g_player_manager->player_leave(player);

		if (net_player_data)
		{
			notify(Info, 3s, "{} has left", net_player_data->m_name);
		}

		return g_hooking->get_original<assign_physical_index>()(netPlayerMgr, player, new_index);
	}

	const auto result = g_hooking->get_original<assign_physical_index>()(netPlayerMgr, player, new_index);

	submenus::g_players_submenu.on_player_join(player);
	g_player_manager->player_join(player);

	if (net_player_data)
	{
		notify(Info, 3s, "{} has joined", net_player_data->m_name);

		auto id = player->m_player_id;

		g_fiber_pool->queue_job([id]
		{
			if (auto plyr = g_player_manager->get_by_id(id))
			{
				if (is_spoofed_host_token(plyr->get_net_data()))
				{
					notify(Warning, 7s, "{} has a spoofed host token: 0x{:X}", plyr->get_name(),
					       plyr->get_net_data()->m_host_token);
				}

				if (g_player_manager->get_self()->is_host() && plyr->get_net_data()->m_nat_type <= 1)
				{
					notify(Warning, 7s, "{} has a NAT type of 0 or 1 (Desync Protection)", plyr->get_name());
				}

				if (plyr->is_host() && plyr->get_net_data()->m_nat_type == 0)
				{
					notify(Warning, 7s, "{} is a host with a NAT type of 0 (Desync Protection)", plyr->get_name());
					// Some broken menus can do this
				}
			}
		});
	}

	return result;
}
