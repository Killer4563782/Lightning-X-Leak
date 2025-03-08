#include "hooking/hooking.hpp"
#include "services/players/player_manager.hpp"
#include "util/globals.h"

unsigned int hooks::broadcast_net_array(rage::netArrayHandlerBase* _this, CNetGamePlayer* target, rage::datBitBuffer* bit_buffer, uint16_t counter, uint32_t* elem_start, bool silent)
{
	int orig_gsbd = 0;

	const auto plyr = g_player_manager->get_by_id(target->m_player_id);

	const bool need_to_use_end_session_kick = plyr && plyr->is_valid()
		&& plyr->trigger_end_session_kick
		&& _this->m_array == scr_globals::gsbd.as<void*>();

	if (need_to_use_end_session_kick)
	{
		orig_gsbd = *scr_globals::gsbd.as<int*>();
		*scr_globals::gsbd.as<uint32_t*>() = 5;
		broadcast_net_array::m_patch->apply();
	}

	const auto ret = g_hooking->get_original<broadcast_net_array>()(_this, target, bit_buffer, counter, elem_start, silent);

	if (need_to_use_end_session_kick)
	{
		broadcast_net_array::m_patch->restore();
		*scr_globals::gsbd.as<int*>() = orig_gsbd;
	}

	return ret;
}
