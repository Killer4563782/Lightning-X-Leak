#pragma once

namespace am_launcher
{
	static const std::unordered_set bad_script_hashes = { "ggsm_arcade"_joaat, "camhedz_arcade"_joaat, "wizard_arcade"_joaat, "puzzle"_joaat, "fm_intro"_joaat, "pilot_school_mp"_joaat, "golf_mp"_joaat, "tennis_network_mp"_joaat, "fm_race_controler"_joaat, "fm_horde_controler"_joaat, "fm_mission_controller"_joaat, "fm_mission_controller_2020"_joaat, "fm_impromptu_dm_controler"_joaat, "fm_deathmatch_controler"_joaat, "fm_bj_race_controler"_joaat, "fm_survival_controller"_joaat, "sctv"_joaat, "am_pi_menu"_joaat, "scroll_arcade_cabinet"_joaat, "grid_arcade_cabinet"_joaat, "degenatron_games"_joaat, "gunslinger_arcade"_joaat, "road_arcade"_joaat, "AM_MP_DRONE"_joaat };

	static void START_NEW_SCRIPT_WITH_ARGS(rage::scrNativeCallContext* src)
	{
		const char* const name = src->get_arg<const char*>(0);

		if (bad_script_hashes.contains(rage::joaat(name)))
		{
			src->set_return_value<int>(0);
			return;
		}

		Any* args = src->get_arg<Any*>(1);
		const int argc = src->get_arg<int>(2);
		const int stackSize = src->get_arg<int>(3);

		src->set_return_value<int>(SYSTEM::START_NEW_SCRIPT_WITH_ARGS(name, args, argc, stackSize));
	}
}
