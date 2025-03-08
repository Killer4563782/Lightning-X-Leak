#include "bones_sub.h"

#include "visuals/visuals.h"
#include "gui/types/color_option.hpp"
#include "gui/types/number_option.hpp"
#include "gui/types/toggle_option.hpp"

using namespace visuals::bones;

namespace ui::submenus
{
	void bones_submenu::on_init()
	{
		this->add(number_option("Distance Limit", "Distance limit for all bones", &max_distance, 7000.f, 0.f, 20.f));
		this->add(toggle_option("Include Self", "Draw the bone lines on the own ped", &include_self));
		this->add(toggle_option("Draw Finger Bones", "Draw finger bones", &draw_finger_bones));

		this->add(toggle_option("Player Bones", "Draw bones on players", &player_bones));
		this->add(color_option("Player Color", "Color of the bone lines when the target is a player", &player_color, true));

		this->add(toggle_option("Ped Bones", "Draw bones on only human peds", &ped_bones));
		this->add(color_option("Ped Color", "Color of the bone lines when the target is a pedestrian", &ped_color, true));
	}
}
