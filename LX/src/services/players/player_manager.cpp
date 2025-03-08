#include "player_manager.hpp"
#include "memory/pointers.h"
#include "network/CNetworkPlayerMgr.hpp"

player_manager::player_manager() :
	m_self(nullptr),
	m_selected_player(m_dummy)
{
	const auto network_player_mgr = *g_pointers->g_network_player_mgr;
	if (!network_player_mgr)
		return;

	m_self = &network_player_mgr->m_local_net_player;

	for (uint16_t i = 0; i < network_player_mgr->m_player_limit; ++i)
		player_join(network_player_mgr->m_player_list[i]);
}

void player_manager::do_cleanup()
{
	m_selected_player = m_dummy;
	m_players.clear();
}

player_ptr player_manager::get_by_msg_id(uint32_t msg_id) const
{
	for (const auto& player : m_players | std::views::values)
	{
		if (const auto net_game_player = player->m_net_game_player)
		{
			if (net_game_player->m_msg_id == msg_id)
			{
				return player;
			}
		}
	}
	return nullptr;
}

player_ptr player_manager::get_by_id(uint32_t id) const
{
	for (const auto& player : m_players | std::views::values)
		if (player && player->id() == id)
			return player;

	return nullptr;
}

player_ptr player_manager::get_by_host_token(uint64_t token) const
{
	for (const auto& player : m_players | std::views::values)
	{
		if (const auto net_data = player->get_net_data())
		{
			if (net_data->m_host_token == token)
			{
				return player;
			}
		}
	}
	return nullptr;
}

player_ptr player_manager::get_selected() const
{
	return m_selected_player;
}

player_ptr player_manager::get_self()
{
	if (!m_self_ptr || !m_self_ptr->equals(*m_self))
		m_self_ptr = std::make_shared<services::player>(*m_self);

	return m_self_ptr;
}

void player_manager::player_join(CNetGamePlayer* net_game_player)
{
	if (net_game_player == nullptr || net_game_player == *m_self)
		return;

	auto plyr = std::make_shared<services::player>(net_game_player);
	m_players.insert({ plyr->get_name(), std::move(plyr) });
}

void player_manager::player_leave(CNetGamePlayer* net_game_player)
{
	if (net_game_player == nullptr)
		return;

	if (m_selected_player && m_selected_player->equals(net_game_player))
		m_selected_player = m_dummy;

	if (auto it = std::ranges::find_if(m_players, [net_game_player](std::pair<const std::string, player_ptr>& p) {
		return p.second->id() == net_game_player->m_player_id;
	}); it != m_players.end())
	{
		m_players.erase(it);
	}
}

void player_manager::set_selected(const player_ptr& plyr)
{
	m_selected_player = plyr;
}
