#include "commands/types/number/looped_float_command.hpp"

namespace commands
{
	class health_regen final : looped_float_command
	{
		using looped_float_command::looped_float_command;
		
		void on_tick() override
		{
			PLAYER::SET_PLAYER_HEALTH_RECHARGE_MAX_PERCENT(self::player, D3D11_FLOAT32_MAX);
			PLAYER::SET_PLAYER_HEALTH_RECHARGE_MULTIPLIER(self::player, m_value);
		}

		void on_disable() override
		{
			PLAYER::SET_PLAYER_HEALTH_RECHARGE_MAX_PERCENT(self::player, 1.f);
			PLAYER::SET_PLAYER_HEALTH_RECHARGE_MULTIPLIER(self::player, 1.f);
		}
	};

	health_regen g_health_regen("Health Regeneration", "health_regen", "Change your health regeneration rate", false, false, 0.f, 10.f, 0.25f, 1.f);
}
