#include "commands/settings.h"
#include "hooking/hooking.hpp"

bool hooks::serialize_player_data_msg(CNetGamePlayerDataMsg* msg, rage::datBitBuffer* buffer)
{
	int old_group = msg->m_matchmaking_group;

	if (commands::settings::misc::join_as_sctv_without_spectating)
		msg->m_matchmaking_group = 4;

	bool ret = g_hooking->get_original<serialize_player_data_msg>()(msg, buffer);
	msg->m_matchmaking_group = old_group;
	return ret;
}
