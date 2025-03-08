#ifdef ENABLE_DEVELOPER_MODE 
#include "particles_submenu.h"

#include "gui/types/break_option.hpp"
#include "gui/types/toggle_option.hpp"
#include "gui/types/function_option.hpp"

static bool m_loop_particles = false;
static void create_particle(const char* fxname, const char* name, const char* effectname)
{
	STREAMING::REQUEST_NAMED_PTFX_ASSET(fxname);
	GRAPHICS::USE_PARTICLE_FX_ASSET(name);
	GRAPHICS::START_PARTICLE_FX_NON_LOOPED_ON_ENTITY(effectname, PLAYER::PLAYER_PED_ID(), 0.0, 0.0, -0.5, 0.0, 0.0, 0.0, 1.0, false, false, false);
}

struct particle_button_option
{
	const char* displayText;
	const char* effectLib1;
	const char* effectLib2;
	const char* effectName;
};

std::vector<particle_button_option> m_particle_options = {
	{"Clown Appears", "scr_rcbarry2", "scr_rcbarry2", "scr_clown_appears"},
	{"Firework 1", "scr_indep_fireworks", "scr_indep_fireworks", "scr_indep_firework_trailburst"},
	{"Firework 2", "proj_xmas_firework", "proj_xmas_firework", "scr_firework_xmas_burst_rgw"},
	{"Firework 3", "proj_xmas_firework", "proj_xmas_firework", "scr_firework_xmas_spiral_burst_rgw"},
	{"Banknotes", "scr_ornate_heist", "scr_ornate_heist", "scr_heist_ornate_banknotes"},
	{"Truck Crash", "scr_fbi4", "scr_fbi4", "scr_fbi4_trucks_crash"},
	{"Ped Sliced", "scr_michael2", "scr_michael2", "scr_abattoir_ped_sliced"},
	{"Camera", "scr_rcpaparazzo1", "scr_rcpaparazzo1", "scr_rcpap1_camera"},
	{"Alien 1", "scr_rcbarry1", "scr_rcbarry1", "scr_alien_disintegrate"},
	{"Alien 2", "scr_rcbarry1", "scr_rcbarry1", "scr_alien_teleport"},
	{"Electric Box", "scr_agencyheistb", "scr_agencyheistb", "scr_agency3b_elec_box"},
	{"Water Splash", "scr_fbi5a", "scr_fbi5a", "scr_fbi5_ped_water_splash"},
	{"Bubbles", "scr_fbi5a", "scr_fbi5a", "water_splash_ped_bubbles"},
	{"Blood Entry", "scr_finalec2", "scr_finalec2", "scr_finale2_blood_entry"},
	{"Sniper Impact", "scr_martin1", "scr_martin1", "scr_sol1_sniper_impact"},
	{"Dirt Throw", "core_snow", "core_snow", "cs_mich1_spade_dirt_throw"},
	{"Blood Stab", "scr_michael2", "scr_michael2", "scr_mich2_blood_stab"},
	{"Meth Smoke", "scr_familyscenem", "scr_familyscenem", "scr_meth_pipe_smoke"},
	{"Burial Dirt", "scr_reburials", "scr_reburials", "scr_burial_dirt"},
	{"Ghost Rider", "scr_agencyheist", "scr_agencyheist", "scr_fbi_dd_breach_smoke"}
};

namespace ui::submenus
{
	void particle_submenu::on_init()
	{
		this->add(toggle_option("Loop Particles", {}, &m_loop_particles));
		for (auto element : m_particle_options)
		{
			this->add(function_option(element.displayText, {}, [&]
			{
				g_fiber_pool->queue_job([&]
				{
					//create_particle(element.effectLib1, element.effectLib2, element.effectName); | Needs to be fixed
				});
			}));
		}
	}
}
#endif