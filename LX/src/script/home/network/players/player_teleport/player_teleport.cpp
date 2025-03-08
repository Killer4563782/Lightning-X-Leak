#include "player_teleport.h"

#include "commands/manager.hpp"
#include "gui/types/function_option.hpp"
#include "helpers/teleportHelpers.h"

namespace ui::submenus
{
	void player_teleport_submenu::on_init()
	{
		this->add(function_option("Teleport To Player", {}, []
		{
			teleport::teleport_to_player(g_player_manager->m_selected_player);
		}));
		this->add(function_option("Teleport Into their Vehicle", {}, []
		{
			teleport::teleport_into_player_vehicle(g_player_manager->m_selected_player);
		}));
		this->add(function_option("Teleport Player to Me", {}, []
		{
			g_fiber_pool->queue_job([]
			{
				teleport::teleport_player_to_me(g_player_manager->m_selected_player);
			});
		}));
		this->add(function_option("Teleport Player to my Waypoint", {}, []
		{
			g_fiber_pool->queue_job([]
			{
				teleport::teleport_player_to_way_point(g_player_manager->m_selected_player);
			});
		}));
	}
}
