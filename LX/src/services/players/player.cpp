#include "player.hpp"
#include "memory/pointers.h"
#include "network/Network.hpp"
#include "player_manager.hpp"
#include "helpers/gxtLabelHelpers.h"
#include "rage/util.h"
#include "script/globals/GPBD_FM.hpp"
#include "services/friends/friends_service.hpp"
#include "services/online/online.h"
#include "util/globals.h"
#include "vehicle/CVehicleModelInfo.hpp"

namespace services
{
	player::player(CNetGamePlayer* net_game_player) : m_net_game_player(net_game_player)
	{
		m_is_friend = friends_service::is_friend(net_game_player);
	}

	CVehicle* player::get_current_vehicle() const
	{
		if (const auto ped = get_ped(); ped != nullptr)
			if (const auto vehicle = ped->m_vehicle; vehicle != nullptr)
				return vehicle;
		return nullptr;
	}

	std::string player::get_vehicle_name() const
	{
		if (auto ped = get_ped(); ped != nullptr)
		{
			if (auto vehicle = ped->m_vehicle)
			{
				auto model = reinterpret_cast<CVehicleModelInfo*>(vehicle->m_model_info);
				auto name = model->m_name;
				return gxt::get_gxt_label(name, true).value_or("INVALID");
			}
		}

		return "Unknown";
	}

	std::string player::get_vehicle_manufacture_name() const
	{
		if (auto ped = get_ped(); ped != nullptr)
		{
			if (auto vehicle = ped->m_vehicle)
			{
				auto model = reinterpret_cast<CVehicleModelInfo*>(vehicle->m_model_info);
				auto manufacturer = model->m_manufacturer;
				return gxt::get_gxt_label(manufacturer).value_or("INVALID");
			}
		}

		return "Unknown";
	}

	rage::fvector3* player::get_pos() const
	{
		if (auto ped = get_ped(); ped && ped->m_navigation)
			return ped->m_navigation->get_position();

		return nullptr;
	}

	const char* player::get_name() const
	{
		return get_net_game_player() == nullptr ? "" : m_net_game_player->get_name();
	}

	rage::rlGamerInfo* player::get_net_data() const
	{
		return get_net_game_player() == nullptr ? nullptr : m_net_game_player->get_net_data();
	}

	CNetGamePlayer* player::get_net_game_player() const
	{
		return m_net_game_player == nullptr || m_net_game_player->m_player_info == nullptr ? nullptr : m_net_game_player;
	}

	int64_t player::get_rockstar_id() const
	{
		if (auto net_data = get_net_data())
			return net_data->m_gamer_handle.m_rockstar_id;
		return 0;
	}

	CPed* player::get_ped() const
	{
		if (auto player_info = get_player_info())
			if (auto ped = player_info->m_ped)
				return ped;
		return nullptr;
	}

	CPlayerInfo* player::get_player_info() const
	{
		if (auto net_player = get_net_game_player())
			return net_player->m_player_info;
		return nullptr;
	}

	rage::snPlayer* player::get_session_player() const
	{
		for (uint32_t i = 0; i < get_network()->m_game_session_ptr->m_player_count; i++)
		{
			if (get_network()->m_game_session_ptr->m_players[i]->m_player_data.m_host_token == get_net_data()->m_host_token)
			{
				return get_network()->m_game_session_ptr->m_players[i];
			}
		}

		if (get_network()->m_game_session_ptr->m_local_player.m_player_data.m_host_token == get_net_data()->m_host_token)
			return &get_network()->m_game_session_ptr->m_local_player;

		return nullptr;
	}

	rage::snPeer* player::get_session_peer() const
	{
		for (uint32_t i = 0; i < get_network()->m_game_session_ptr->m_peer_count; i++)
		{
			if (get_network()->m_game_session_ptr->m_peers[i]->m_peer_data.m_gamer_handle.m_rockstar_id == get_rockstar_id())
			{
				return get_network()->m_game_session_ptr->m_peers[i];
			}
		}

		return nullptr;
	}

	rage::netConnectionPeer* player::get_connection_peer() const
	{
		if (get_session_player())
			if (auto peer = g_pointers->g_get_connection_peer(get_network()->m_game_session_ptr->m_net_connection_mgr,
			        static_cast<int>(get_session_player()->m_player_data.m_peer_id_2)))
				return peer;

		return nullptr;
	}

	std::optional<netAddress> player::get_ip_address() const
	{
		if (this == g_player_manager->get_self().get() && get_net_data())
			return get_net_data()->m_external_ip;

		if (auto peer = get_connection_peer())
		{
			if (peer->m_peer_address.m_connection_type != 1)
				return std::nullopt;

			return peer->m_peer_address.m_external_ip;
		}

		return std::nullopt;
	}

	uint16_t player::get_port() const
	{
		if (this == g_player_manager->get_self().get() && get_net_data())
			return get_net_data()->m_external_port;

		if (auto peer = get_connection_peer())
		{
			if (peer->m_peer_address.m_connection_type != 1)
				return 0;

			return peer->m_peer_address.m_external_port;
		}

		return 0;
	}

	uint8_t player::id() const
	{
		if (get_network_player_mgr()->m_player_count > 0)
			return get_net_game_player() == nullptr ? -1 : m_net_game_player->m_player_id;
		return self::player;
	}

	int player::rank() const
	{
		if (const auto& stats = scr_globals::gpbd_fm_1.as<GPBD_FM*>()->Entries[id()].PlayerStats; is_valid() && stats.Rank >= 1 && stats.Rank <= 8000)
			return stats.Rank;

		return -1;
	}

	bool player::is_host() const
	{
		return get_net_game_player() == nullptr ? false : m_net_game_player->is_host();
	}

	bool player::is_friend() const
	{
		return m_is_friend;
	}

	bool player::is_valid() const
	{
		return get_net_game_player() == nullptr ? false : m_net_game_player->is_valid();
	}

	bool player::is_script_host() const
	{
		return m_net_game_player && get_script_host() == m_net_game_player;
	}

	bool player::equals(const CNetGamePlayer* net_game_player) const
	{
		return net_game_player == get_net_game_player();
	}

	std::string player::to_lowercase_identifier() const
	{
		std::string lower = get_name();
		std::ranges::transform(lower, lower.begin(), ::tolower);

		return lower;
	}
}
