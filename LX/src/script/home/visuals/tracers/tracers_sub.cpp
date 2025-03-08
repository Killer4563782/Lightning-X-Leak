#include "tracers_sub.h"

#include "visuals/visuals.h"
#include "gui/types/color_option.hpp"
#include "gui/types/number_option.hpp"
#include "gui/types/toggle_option.hpp"

using namespace visuals::tracers;

namespace ui::submenus
{
	void tracers_submenu::on_init()
	{
		this->add(number_option("Pos X", "X position on your screen where tracers should start. \nOnly when 3D Tracers is disabled", &start_pos.x, 1.f, 0.f, 0.1f));
		this->add(number_option("Pos Y", "Y position on your screen where tracers should start. \nOnly when 3D Tracers is disabled", &start_pos.y, 1.f, 0.f, 0.1f));
		this->add(number_option("Distance Limit", "Distance limit for all tracers", &max_distance, 7000.f, 0.f, 20.f));
		this->add(toggle_option("3D Tracers", "Draw the tracer lines in game instead of on screen", &use_native_3d));
		this->add(toggle_option("Include Self", "Draw the tracer lines to own ped and vehicle", &include_self));

		this->add(toggle_option("Player Tracers", "Draw tracers to players", &player_tracers));
		this->add(color_option("Player Color", "Color of the tracer line when the target is a player", &player_color, true));

		this->add(toggle_option("Ped Tracers", "Draw tracers to ped", &ped_tracers));
		this->add(toggle_option("Only Humans", "Only draw tracers to human peds", &only_humans));
		this->add(color_option("Ped Color", "Color of the tracer line when the target is a pedestrian", &ped_color, true));
		
		// These below can be removed tbh
		this->add(toggle_option("Vehicle Tracers", "Draw tracers to vehicles", &vehicle_tracers));
		this->add(color_option("Vehicle Color", "Color of the tracer line when the target is a vehicle", &vehicle_color, true));

		this->add(toggle_option("Props Tracers", "Draw tracers to props", &prop_tracers));
		this->add(color_option("Props Color", "Color of the tracer line when the target is a prop", &prop_color, true));
	}
}
