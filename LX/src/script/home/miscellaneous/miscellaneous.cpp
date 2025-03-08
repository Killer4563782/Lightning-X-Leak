#include "miscellaneous.h"
#include "vision_submenu.hpp"
#include "commands/manager.hpp"
#include "commands/settings.h"
#include "gui/types/function_option.hpp"
#include "gui/types/toggle_option.hpp"
#include "commands/commands/misc/fake_money.hpp"

namespace ui::submenus
{
	void miscellaneous_submenu::on_init()
	{
		this->add(function_option("Skip Cutscenes", {}, [&]
		{
			g_fiber_pool->queue_job([&]
			{
				if (CUTSCENE::IS_CUTSCENE_PLAYING())
				{
					const auto get = CUTSCENE::GET_CUTSCENE_SECTION_PLAYING();
					CUTSCENE::SET_CUTSCENE_CAN_BE_SKIPPED(get);
					CUTSCENE::STOP_CUTSCENE(get);
					CUTSCENE::STOP_CUTSCENE_IMMEDIATELY();
					CUTSCENE::REMOVE_CUTSCENE();
				}
				else
				{
					notify(Error, 5s, "No cutscene is playing or it cannot be skipped");
				}
			});
		}));
		this->add(vision_submenu("Screen Effects", {}));
		this->add(toggle_option("money_seperator"_BC));
		this->add(commands::fake_money_option("Fake Money", "Display a fake amount of money", &commands::settings::misc::fake_money_enabled, &commands::settings::misc::fake_money, LLONG_MAX, LLONG_MIN));
		this->add(function_option("restart_game"_C));
		this->add(toggle_option("console"_BC));
		this->add(toggle_option("instant_close"_LC));
		this->add(function_option("force_quit_to_singleplayer"_C));
		this->add(function_option("restart_game"_C));
		this->add(function_option("Unload", {}, []
		{
			g_running = false;
		}));
		this->add(function_option("Exit", {}, []
		{
			g_running = false;
			TerminateProcess(GetCurrentProcess(), 0);
		}));
	}

	miscellaneous_submenu g_miscellaneous_submenu("Miscellaneous", "Miscellaneous Options");
}
