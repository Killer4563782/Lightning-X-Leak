#include "commands/types/looped_command.hpp"
#include "memory/pointers.h"

namespace commands
{
	class drift_mode final : looped_command
	{
		using looped_command::looped_command;

		bool m_prev_drift_tyres = false;
		float m_prev_steering_lock = 5.f;
		CVehicle* m_last_vehicle;

		void on_tick() override
		{
			if (!self::local_ped)
				return;

			if (auto vehicle = self::local_ped->m_vehicle; vehicle && vehicle != m_last_vehicle)
			{
				if (Vehicle handle = g_pointers->g_pointer_to_handle(vehicle); !VEHICLE::GET_DRIFT_TYRES_SET(handle))
				{
					m_prev_drift_tyres = false;
					VEHICLE::SET_DRIFT_TYRES(handle, TRUE);
				}
				else m_prev_drift_tyres = true;

				// Similar to stand drift mode
				m_prev_steering_lock = vehicle->m_handling_data->m_steering_lock;
				vehicle->m_handling_data->m_steering_lock = m_prev_steering_lock * 2;

				m_last_vehicle = vehicle;
			}
			else if (m_last_vehicle && vehicle != m_last_vehicle)
			{
				if (Vehicle handle = g_pointers->g_pointer_to_handle(m_last_vehicle); ENTITY::DOES_ENTITY_EXIST(handle))
					VEHICLE::SET_DRIFT_TYRES(handle, m_prev_drift_tyres);

				m_last_vehicle->m_handling_data->m_steering_lock = m_prev_steering_lock;
				m_prev_steering_lock = 5.f;
				m_last_vehicle = nullptr;
			}
		}

		void on_disable() override
		{
			if (m_last_vehicle)
			{
				if (Vehicle handle = g_pointers->g_pointer_to_handle(m_last_vehicle); ENTITY::DOES_ENTITY_EXIST(handle))
					VEHICLE::SET_DRIFT_TYRES(handle, m_prev_drift_tyres);

				m_last_vehicle->m_handling_data->m_steering_lock = m_prev_steering_lock;
				m_prev_steering_lock = 5.f;
				m_last_vehicle = nullptr;
			}
		}
	};

	drift_mode g_drift_mode("Drift Mode", "drift_mode", {});
}
