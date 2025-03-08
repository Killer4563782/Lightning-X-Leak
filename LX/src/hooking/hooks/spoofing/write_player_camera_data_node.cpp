#include "commands/manager.hpp"
#include "hooking/hooking.hpp"

void hooks::write_player_camera_data_node(rage::netObject* player, CPlayerCameraDataNode* node)
{
	g_hooking->get_original<write_player_camera_data_node>()(player, node);

	if ("spectate"_LC->m_toggle || "free_cam"_LFC->m_toggle)
	{
		node->m_free_cam_pos_x += 100.f;
		node->m_free_cam_pos_y -= 100.f;
		node->m_camera_x -= 100.f;
	}
}
