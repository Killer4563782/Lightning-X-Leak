#include "commands/manager.hpp"
#include "helpers/networkHelpers.h"
#include "hooking/hooking.hpp"

void hooks::write_player_game_state_data_node(rage::netObject* player, CPlayerGameStateDataNode* node)
{
	g_hooking->get_original<write_player_game_state_data_node>()(player, node);

	if ("godmode"_BC->m_toggle && !network_helpers::is_in_cutscene(g_player_manager->get_self()) && !network_helpers::is_in_interior(g_player_manager->get_self()))
	{
		node->m_is_invincible   = false;
		node->m_bullet_proof    = false;
		node->m_collision_proof = false;
		node->m_explosion_proof = false;
		node->m_fire_proof      = false;
		node->m_melee_proof     = false;
		node->m_steam_proof     = false;
		node->m_water_proof     = false;
	}

	if ("super_jump"_LVC->m_toggle)
	{
		node->m_super_jump = false;
	}

	if ("spectate"_BC->m_toggle)
	{
		node->m_is_spectating     = false;
		node->m_spectating_net_id = 0;
	}
}
