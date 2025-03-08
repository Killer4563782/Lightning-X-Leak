#include "commands/types/looped_command.hpp"

namespace commands
{
	class vehicle_godmode final : looped_command
	{
		using looped_command::looped_command;

		CVehicle* m_last_vehicle;

		void on_tick() override
		{
			if (!self::local_ped)
				return;

			if (auto vehicle = self::local_ped->m_vehicle; vehicle && m_last_vehicle != vehicle)
			{
				if (m_last_vehicle)
				{
					m_last_vehicle->m_deform_god &= ~(1 << 4);
					m_last_vehicle->m_damage_bits &= ~God;
					m_last_vehicle = nullptr;
				}

				m_last_vehicle = vehicle;
				vehicle->m_deform_god |= 1 << 4;
				vehicle->m_damage_bits |= God;
			}
		}

		void on_disable() override
		{
			if (!self::local_ped || !m_last_vehicle)
				return;

			m_last_vehicle->m_deform_god &= ~(1 << 4);
			m_last_vehicle->m_damage_bits &= ~God;
			m_last_vehicle = nullptr;
		}
	};

	vehicle_godmode g_vehicle_godmode("Vehicle Godmode", "vehicle_godmode", "Your current or last driven vehicle can not be destroyed");
}
