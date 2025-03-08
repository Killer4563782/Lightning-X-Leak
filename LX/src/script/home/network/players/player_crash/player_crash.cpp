#include "player_crash.h"
#include "gui/types/function_option.hpp"
#include "helpers/networkHelpers.h"

namespace ui::submenus
{
	void player_crash_submenu::on_init()
	{
		this->add(function_option("Ped Crash", {}, []
		{
			crashes::ped_crash(g_player_manager->m_selected_player);
		}));
		/*this->add(function_option("Crash 1 [TESTING]", {}, []
		{
			crashes::crash_1(g_player_manager->m_selected_player);
		}));*/
		/*this->add(function_option("Crash 2 [TESTING]", {}, []
		{
			crashes::crash_2(g_player_manager->m_selected_player);
		}));*/
		this->add(function_option("9/11 Crash", "Note: This will crash the entire session", []
		{
			crashes::crash_3(g_player_manager->m_selected_player);
		}));
		/*this->add(function_option("Crash 4 [TESTING]", {}, []
		{
			crashes::crash_4(g_player_manager->m_selected_player);
		}));*/
		this->add(function_option("Drunk Driver", {}, []
		{
			crashes::crash_5(g_player_manager->m_selected_player);
		}));
		this->add(function_option("Chandelier", {}, []
		{
			crashes::crash_6(g_player_manager->m_selected_player);
		}));
		/*this->add(function_option("Crash 7 [TESTING]", {}, []
		{
			crashes::crash_7(g_player_manager->m_selected_player);
		}));*/
		/*this->add(function_option("Crash 8 [TESTING]", {}, []
		{
			crashes::crash_8(g_player_manager->m_selected_player);
		}));*/
		/*this->add(function_option("Crash 9 [TESTING]", {}, []
		{
			crashes::crash_9(g_player_manager->m_selected_player);
		}));*/
	}
}