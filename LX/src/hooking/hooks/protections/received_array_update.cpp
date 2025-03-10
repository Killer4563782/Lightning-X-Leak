#include "util/globals.h"
#include "hooking/hooking.hpp"
#include "rage/util.h"
#include "services/players/player_manager.hpp"
#include "services/reactions/reactions.h"
#include "util/script_global.hpp"

bool hooks::received_array_update(rage::netArrayHandlerBase* array, CNetGamePlayer* sender, rage::datBitBuffer* buffer, int size, std::int16_t cycle)
{
	using namespace scr_locals;

	int old_beast_index = -1;
	int participant_id = 0;
	const auto beast = find_script_thread("am_hunt_the_beast"_joaat);

	if (beast)
	{
		old_beast_index = *script_local(beast->m_stack, am_hunt_the_beast::broadcast_idx).at(1).at(6).as<int*>();
		if (beast->m_net_component)
			participant_id = reinterpret_cast<CGameScriptHandlerNetComponent*>(beast->m_net_component)->m_local_participant_index;
	}

	const bool result = g_hooking->get_original<received_array_update>()(array, sender, buffer, size, cycle);

	if (beast && array->m_array == script_local(beast->m_stack, am_hunt_the_beast::broadcast_idx).as<void*>()
		&& old_beast_index
		!= *script_local(beast->m_stack, am_hunt_the_beast::broadcast_idx).at(1).at(6).as<int*>()
		&& *script_local(beast->m_stack, am_hunt_the_beast::broadcast_idx).at(1).at(6).as<int*>() == participant_id
		&& *script_local(beast->m_stack, am_hunt_the_beast::player_broadcast_idx).at(participant_id, 4).at(3).as<int*>() & 1 << 1)
	{
		*script_local(beast->m_stack, am_hunt_the_beast::broadcast_idx).at(1).at(6).as<int*>() = -1;
		*script_local(beast->m_stack, am_hunt_the_beast::broadcast_idx).at(1).at(7).as<Player*>() = -1;

		if (const auto plyr = g_player_manager->get_by_id(sender->m_player_id))
			return turn_into_beast.process(plyr, "Turn Into Beast");
	}

	if (array->m_array >= scr_globals::globalplayer_bd.as<uint8_t*>()
		&& array->m_array <= scr_globals::globalplayer_bd.at(31, sizeof(GlobalPlayerBDEntry) / 8).as<uint8_t*>()
		&& scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[sender->m_player_id].RemoteWantedLevelPlayer != -1)
	{
		if (const auto plyr = g_player_manager->get_by_id(sender->m_player_id))
		{
			if (scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[sender->m_player_id].RemoteWantedLevelPlayer == self::player)
			{
				if (remote_wanted_level.process(plyr, "Remote Wanted Level"))
					scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[sender->m_player_id].RemoteWantedLevelPlayer = -1; // reset locally
			}
		}

	}

	if (array->m_array == scr_globals::gsbd.as<void*>() && *scr_globals::gsbd.as<eFreemodeState*>() == eFreemodeState::CLOSING)
	{
		if (const auto plyr = g_player_manager->get_by_id(sender->m_player_id); kick.process(plyr, "Kick A8")) // End session kick
			*scr_globals::gsbd.as<eFreemodeState*>() = eFreemodeState::RUNNING;

	}

	return result;
}
