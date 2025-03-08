#include "spoofing.h"
#include "gui/types/toggle_option.hpp"
#include "commands/settings.h"
#include "commands/manager.hpp"

namespace ui::submenus
{
	void spoofing_submenu::on_init()
	{
		this->add(toggle_option("Spoof Godmode", "This will prevent other modders identifying you as godmode", &commands::settings::spoofing::spoof_godmode));
		this->add(toggle_option("Spoof Spectate", "This will prevent other modders knowing that you are spectating someone", &commands::settings::spoofing::spoof_spectate));
		this->add(toggle_option("force_relay"_LC));
		this->add(toggle_option("override_game_hashes"_LC));
	}

	spoofing_submenu g_spoofing_submenu("Spoofing", "Spoofing options");
}
