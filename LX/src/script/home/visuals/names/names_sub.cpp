#include "names_sub.h"

#include "visuals/visuals.h"
#include "gui/types/color_option.hpp"
#include "gui/types/number_option.hpp"
#include "gui/types/toggle_option.hpp"

using namespace visuals::names;

namespace ui::submenus
{
	void names_submenu::on_init()
	{
		this->add(number_option("Distance Limit", "Distance limit for all names", &max_distance, 7000.f, 0.f, 20.f));
		this->add(toggle_option("Include Self", "Draw name above the own ped", &include_self));
		this->add(toggle_option("Show Tags", "Draw the tags next to the name (only online)", &show_tags));
		this->add(toggle_option("Player Names", "Draw names above players", &player_names));
		this->add(color_option("Player Color", "Color of the name text", &player_color, true));

		// Could add vehicle so that it shows the vehicle names above vehicles
	}
}
