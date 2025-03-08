#include "commands/types/bool_command.hpp"

namespace commands
{
	class weapons_in_interior final : bool_command
	{
		using bool_command::bool_command;

		void on_enable() override
		{
			constexpr int controls[] = {
				INPUT_ATTACK,
				INPUT_AIM,
				INPUT_SELECT_WEAPON,
				INPUT_DETONATE,
				INPUT_WEAPON_SPECIAL,
				INPUT_WEAPON_SPECIAL_TWO,
				INPUT_VEH_AIM,
				INPUT_VEH_ATTACK,
				INPUT_VEH_ATTACK2,
				INPUT_VEH_PASSENGER_AIM,
				INPUT_VEH_PASSENGER_ATTACK,
				INPUT_VEH_SELECT_NEXT_WEAPON,
				INPUT_VEH_SELECT_PREV_WEAPON,
				INPUT_VEH_FLY_ATTACK,
				INPUT_MELEE_ATTACK_LIGHT,
				INPUT_MELEE_ATTACK_HEAVY,
				INPUT_MELEE_ATTACK_ALTERNATE,
				INPUT_MELEE_BLOCK,
				INPUT_SELECT_WEAPON_UNARMED,
				INPUT_SELECT_WEAPON_MELEE,
				INPUT_SELECT_WEAPON_HANDGUN,
				INPUT_SELECT_WEAPON_SHOTGUN,
				INPUT_SELECT_WEAPON_SMG,
				INPUT_SELECT_WEAPON_AUTO_RIFLE,
				INPUT_SELECT_WEAPON_SNIPER,
				INPUT_SELECT_WEAPON_HEAVY,
				INPUT_SELECT_WEAPON_SPECIAL,
				INPUT_ATTACK2,
				INPUT_MELEE_ATTACK1,
				INPUT_MELEE_ATTACK2,
				INPUT_VEH_GUN_LEFT,
				INPUT_VEH_GUN_RIGHT,
				INPUT_VEH_GUN_UP,
				INPUT_VEH_GUN_DOWN,
				INPUT_VEH_MELEE_HOLD,
				INPUT_VEH_MELEE_LEFT,
				INPUT_VEH_MELEE_RIGHT,
			};

			for (const auto control : controls)
			{
				PAD::ENABLE_CONTROL_ACTION(0, control, TRUE);
			}
		}
	};

	weapons_in_interior g_weapons_in_interior("Weapons in Interior", "weapons_in_interior", "Allows you use weapons inside of interiors");
}