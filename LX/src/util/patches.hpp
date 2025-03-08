#pragma once
#include "util.h"
#include "memory/byte_patch.hpp"

namespace world_model
{
	inline memory::byte_patch* world_model_spawn_bypass;

	inline Object spawn(Hash hash, const Vector3& location, bool is_networked = true)
	{
		if (!util::natives::request_model(hash))
		{
			LOG_DEBUG("Failed to load model 0x{:X}", hash);
			return 0;
		}

		world_model_spawn_bypass->apply();
		const auto object = OBJECT::CREATE_OBJECT(hash, location.x, location.y, location.z, is_networked, false, false);
		world_model_spawn_bypass->restore();

		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
		return object;
	}
}

namespace explosion_anti_cheat_bypass
{
	inline memory::byte_patch* can_blame_others;
	inline memory::byte_patch* can_use_blocked_explosions;
	inline memory::byte_patch* m_set_script_flag;

	inline void apply()
	{
		can_blame_others->apply();
		can_use_blocked_explosions->apply();
		m_set_script_flag->apply();
	}

	inline void restore()
	{
		can_use_blocked_explosions->restore();
		can_blame_others->restore();
		m_set_script_flag->restore();
	}

	inline void explode(Player explosion_owner, Vector3 pos, eExplosionTag explosion_type, float damage_scale = 1.f, bool audible = true, bool visible = true, float camera_shake = 1.f)
	{
		apply();

		FIRE::ADD_OWNED_EXPLOSION(
			explosion_owner,
			pos.x, pos.y, pos.z,
			explosion_type,
			damage_scale,
			audible,
			visible,
			camera_shake
		);

		restore();
	}
};

namespace police
{
	inline static memory::byte_patch* max_wanted_level;
	inline static memory::byte_patch* max_wanted_level_2;
};
