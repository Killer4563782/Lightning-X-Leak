#include "online.h"
#include "rage/util.h"
#include "services/api/api_service.hpp"
#include "services/players/player_manager.hpp"

namespace services
{
	CNetGamePlayer* get_local_player()
	{
		if (const auto mgr = get_network_player_mgr())
			return mgr->m_local_net_player;

		return nullptr;
	}

	bool is_online()
	{
		return *g_pointers->g_is_session_active;
	}

	CNetGamePlayer* get_host()
	{
		if (is_online())
		{
			for (uint32_t i = 0; i < get_network_player_mgr()->m_player_limit; i++)
			{
				if (auto player = get_network_player_mgr()->m_player_list[i]; player && player->is_host())
					return player;
			}
		}

		return nullptr;
	}

	CNetGamePlayer* get_script_host()
	{
		if (auto thr = find_script_thread("freemode"_joaat); thr && thr->m_net_component)
			return reinterpret_cast<CGameScriptHandlerNetComponent*>(thr->m_net_component)->get_host();

		return nullptr;
	}


	CNetGamePlayer** get_players()
	{
		if (is_online())
			return get_network_player_mgr()->m_player_list;

		return nullptr;
	}

	uint16_t get_player_limit()
	{
		if (is_online())
			return get_network_player_mgr()->m_player_limit;

		return 0;
	}

	uint16_t get_player_count()
	{
		if (is_online())
			return get_network_player_mgr()->m_player_count;

		return 0;
	}


	FriendInfo* get_friend(uint32_t index)
	{
		if (const auto friends_reg = g_pointers->g_friend_registry)
			return friends_reg->m_friend_count > index ? friends_reg->get(index) : nullptr;

		return nullptr;
	}

	uint32_t get_friends_count()
	{
		if (const auto friends_reg = g_pointers->g_friend_registry)
			return friends_reg->m_friend_count;

		return 0;
	}

	bool is_friend(CNetGamePlayer* net_player)
	{
		if (net_player == nullptr)
			return false;

		if (auto net_data = net_player->get_net_data())
		{
			auto rockstar_id = net_data->m_gamer_handle.m_rockstar_id;
			for (uint32_t i = 0; i < g_pointers->g_friend_registry->m_friend_count; i++)
				if (rockstar_id == g_pointers->g_friend_registry->get(i)->m_rockstar_id)
					return true;
		}

		return false;
	}

	void open_profile_by_rid(uint64_t rid)
	{
		g_open = false;
		rage::rlGamerHandle player_handle(rid);
		g_pointers->g_show_profile_by_gamer_handle(&player_handle);
	}

	void give_collectible(Player target, eCollectibleType col, int index, bool uncomplete)
	{
		constexpr size_t arg_count = 8;
		int64_t args[arg_count] = {
			static_cast<int64_t>(eScriptEvents::ScriptEventCollectibleCollected),
			static_cast<int64_t>(g_player_manager->get_self()->id()),
			1 << target,
			static_cast<int64_t>(col), // iParam0
			static_cast<int64_t>(index), // iParam1
			1, // bParam2
			1,
			1 // bParam3
		};

		g_pointers->g_trigger_script_event(1, args, arg_count, 1 << target, static_cast<int>(eScriptEvents::ScriptEventCollectibleCollected));
	}
}
