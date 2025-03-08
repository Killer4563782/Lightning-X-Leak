#include "player_kick.h"
#include "commands/manager.hpp"
#include "gui/types/function_option.hpp"

namespace ui::submenus
{
	void player_kick_submenu::on_init()
	{
		this->add(function_option("Smart", "Removing a player discreetly without being caught red-handed", []
		{
			"smart_kick"_PC->call(g_player_manager->get_selected(), {});
		}));
		this->add(function_option("Host", "Removes a player using host privileges and adds them to a blacklist, preventing further joining" , []
		{
			"host_kick"_PC->call(g_player_manager->get_selected(), {});
		}));
		this->add(function_option("Desync", "Removes a player through the complaint mechanism without disclosing the sender of the complaint", []
		{
			"desync_kick"_PC->call(g_player_manager->get_selected(), {});
		}));
		this->add(function_option("McDonald's", "Removes a player using the McDonald's wifi connection method", []
		{
			"oom_kick"_PC->call(g_player_manager->get_selected(), {});
		}));
		this->add(function_option("Secretary", "Requesting the host's secretary to remove a player", []
		{
			"script_host_kick"_PC->call(g_player_manager->get_selected(), {});
		}));
		this->add(function_option("Store Closed", "Removing a player by reporting them to a store manager", []
		{
			"end_session_kick"_PC->call(g_player_manager->get_selected(), {});
		}));
	}
}
