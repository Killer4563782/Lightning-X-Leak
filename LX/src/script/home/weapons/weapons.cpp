#include "weapons.h"

#include "commands/manager.hpp"
#include "gui/types/break_option.hpp"
#include "gui/types/function_option.hpp"
#include "gui/types/toggle_option.hpp"
#include "gui/types/vector_bool_option.hpp"
#include "util/patches.hpp"

namespace ui::submenus
{
	void weapons_submenu::on_init()
	{
		this->add(function_option("get_all_weapons"_C));
		this->add(function_option("remove_all_weapons"_C));
		this->add(toggle_option("weapons_in_interior"_BC));
		this->add(toggle_option("no_recoil"_LC));
		this->add(toggle_option("no_spread"_LC));
		this->add(toggle_option("silent_aimbot"_BC));
		this->add(toggle_option("magic_bullets"_BC));
		this->add(vector_bool_option("trigger_bot"_LVC));
		this->add(toggle_option("inf_clip"_LC));
		this->add(toggle_option("inf_ammo"_LC));
		this->add(toggle_option("rapid_fire"_LC));
		this->add(toggle_option("no_limit"_LC));

		this->add(break_option("Custom Weapons"));
		this->add(function_option("Katana", "Switch to a different weapon to remove the katana", []
		{
			static Object katana;
			if (katana != NULL)
				return;

			g_fiber_pool->queue_job([&]
			{
				WEAPON::GIVE_WEAPON_TO_PED(self::ped, "weapon_knife"_joaat, 0, TRUE, TRUE);
				ENTITY::SET_ENTITY_VISIBLE(WEAPON::GET_CURRENT_PED_WEAPON_ENTITY_INDEX(self::ped, FALSE), FALSE, FALSE);

				katana = world_model::spawn("prop_cs_katana_01"_joaat, { 0.f, 0.f, 0.f });
				ENTITY::ATTACH_ENTITY_TO_ENTITY(katana, self::ped, PED::GET_PED_BONE_INDEX(self::ped, SKEL_R_Hand), 0.1f, 0.04f, -0.02f, -75.f, 5.f, -5.f, false, false, false, false, 2, true, false);

				Hash weapon_hash;
				while (WEAPON::GET_CURRENT_PED_WEAPON(self::ped, &weapon_hash, FALSE) && weapon_hash == "weapon_knife"_joaat)
					script::get_current()->yield(500ms);

				ENTITY::SET_ENTITY_VISIBLE(katana, FALSE, FALSE);
				OBJECT::DELETE_OBJECT(&katana);
				katana = NULL;
			});
		}));
		this->add(function_option("Dildo", "Switch to a different weapon to remove the dildo", []
		{
			static Object dildo;
			if (dildo != NULL)
				return;

			g_fiber_pool->queue_job([&]
			{
				WEAPON::GIVE_WEAPON_TO_PED(self::ped, "weapon_dagger"_joaat, 0, TRUE, TRUE);
				ENTITY::SET_ENTITY_VISIBLE(WEAPON::GET_CURRENT_PED_WEAPON_ENTITY_INDEX(self::ped, FALSE), FALSE, FALSE);

				dildo = world_model::spawn("prop_cs_dildo_01"_joaat, { 0.f, 0.f, 0.f });
				ENTITY::ATTACH_ENTITY_TO_ENTITY(dildo, self::ped, PED::GET_PED_BONE_INDEX(self::ped, SKEL_R_Hand), 0.1f, 0.04f, -0.02f, -75.f, 5.f, -5.f, false, false, false, false, 2, true, false);

				Hash weapon_hash;
				while (WEAPON::GET_CURRENT_PED_WEAPON(self::ped, &weapon_hash, FALSE) && weapon_hash == "weapon_dagger"_joaat)
					script::get_current()->yield(500ms);

				ENTITY::SET_ENTITY_VISIBLE(dildo, FALSE, FALSE);
				OBJECT::DELETE_OBJECT(&dildo);
				dildo = NULL;
			});
		}));
		this->add(toggle_option("rocket_gun"_LC));
		this->add(toggle_option("airstrike_gun"_LC));
		this->add(toggle_option("teleport_gun"_LC));
		this->add(toggle_option("minecraft_gun"_LC));
		this->add(toggle_option("cage_gun"_LC));
		this->add(toggle_option("ptfx_gun"_LC));
		this->add(toggle_option("cartoon_gun"_LC));
		this->add(toggle_option("rgb_gun"_LC));
	}

	weapons_submenu g_weapons_submenu("Weapons", {});
}
