#include "commands/types/looped_command.hpp"
#include "util/util.h"

namespace commands
{
	class cartoon_gun final : looped_command
	{
		using looped_command::looped_command;

		void on_tick() override
		{
			Hash weapon_hash; 
			if (!WEAPON::GET_CURRENT_PED_WEAPON(self::ped, &weapon_hash, false) || !PED::IS_PED_SHOOTING(self::ped))
				return;
			
			if (!util::natives::request_named_ptfx_asset("scr_rcbarry2"))
				return;

			const auto weapon_index = WEAPON::GET_CURRENT_PED_WEAPON_ENTITY_INDEX(self::ped, 0);
			Vector3 v0{};
			Vector3 v1{};
			MISC::GET_MODEL_DIMENSIONS(WEAPON::GET_SELECTED_PED_WEAPON(self::ped), &v0, &v1);
			GRAPHICS::USE_PARTICLE_FX_ASSET("scr_rcbarry2");
			GRAPHICS::START_NETWORKED_PARTICLE_FX_NON_LOOPED_ON_PED_BONE("muz_clown", weapon_index, (v0.x - v1.x) / 2.0f + 0.7f, 0.f, 0.f, 0.f, 180.f, 0.f, 1, 0.5f, TRUE, TRUE, TRUE);
		}

		void on_disable() override
		{
			STREAMING::REMOVE_NAMED_PTFX_ASSET("scr_rcbarry2");
		}
	};

	cartoon_gun g_cartoon_gun("Cartoon Gun", "cartoon_gun", {});
}
