#include "commands/types/looped_command.hpp"

namespace commands
{
	class seatbealt final : looped_command
	{
		using looped_command::looped_command;

		void on_tick() override
		{
			PED::SET_PED_CONFIG_FLAG(self::ped, 32, false);
			PED::SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(self::ped, 1);
		}

		void on_disable() override
		{
			PED::SET_PED_CONFIG_FLAG(self::ped, 32, false);
			PED::SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(self::ped, 0);
		}
	};

	seatbealt g_seatbealt("Seatbealt", "seatbealt", "You wont fall off vehicles anymore");
}
