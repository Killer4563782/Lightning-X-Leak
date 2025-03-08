#include "hooking/hooking.hpp"
#include "memory/pointers.h"

bool hooks::handle_join_request(Network* network, rage::snSession* session, rage::rlGamerInfo* player_info, CJoinRequestContext* ctx, BOOL is_transition_session)
{
	notify(Info, 5s, "{} is joining you", player_info->m_name);
	return g_hooking->get_original<handle_join_request>()(network, session, player_info, ctx, is_transition_session);
}
