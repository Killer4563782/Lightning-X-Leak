#include "vision_submenu.hpp"

#include "gui/types/break_option.hpp"
#include "gui/types/toggle_option.hpp"
#include "gui/types/function_option.hpp"

std::vector<std::pair<std::string, std::string>> m_visions = {
	{ "Damage", "damage" },
	{ "Vagos", "VagosSPLASH" },
	{ "Cops", "CopsSPLASH" },
	{ "White screen", "BarryFadeOut" },
	{ "Water lab", "WATER_lab" },
	{ "MP spectator cam", "Multipayer_spectatorCam" },
	{ "Cops", "cops" },
	{ "Spectator 1", "spectator1" },
	{ "Sunglasses", "sunglasses" },
	{ "Dying", "dying" },
	{ "Orange", "REDMIST" },
	{ "Vagos", "vagos" },
	{ "Blurry", "CHOP" },
	{ "Stoned", "stoned" },
	{ "Prologue shootout", "prologue_shootout" },
	{ "Secret camera", "secret_camera" },
	{ "UFO", "ufo" },
	{ "UFO deathray", "ufo_deathray" },
	{ "Wobbly", "drug_wobbly" },
	{ "Killstreak", "MP_Killstreak" },
	{ "Hint cam", "Hint_cam" },
	{ "Black and white", "blackNwhite" },
	{ "Sniper", "sniper" },
	{ "Crane cam", "crane_cam" },
	{ "Bikers", "BikersSPLASH" }
};

namespace ui::submenus
{
	void vision_submenu::on_init()
	{
		this->add(function_option("Reset", {}, [&]
		{
			g_fiber_pool->queue_job([&]
			{
				GRAPHICS::CLEAR_TIMECYCLE_MODIFIER();
			});
		}));
		for (const auto& vision : m_visions)
		{
			this->add(function_option(vision.first, {}, [&]
			{
				g_fiber_pool->queue_job([&]
				{
					GRAPHICS::SET_TIMECYCLE_MODIFIER(vision.second.c_str());
				});
			}));
		}
	}

	vision_submenu g_vision_submenu("Visions", {});
}