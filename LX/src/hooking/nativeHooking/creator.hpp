#pragma once
#include "native_hooks.hpp"

namespace creator
{
	inline void GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(rage::scrNativeCallContext* src)
	{
		const auto hash = src->get_arg<rage::joaat_t>(0);

		if (hash == "freemode"_joaat || hash == "main"_joaat)
		{
			src->set_return_value(0);
			return;
		}

		src->set_return_value(SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(hash));
	}

	inline void GET_ENTITY_MODEL(rage::scrNativeCallContext* src)
	{
		Entity entity = src->get_arg<Entity>(0);
		Hash model = ENTITY::GET_ENTITY_MODEL(entity);

		if (entity == self::ped)
		{
			if (model != "mp_m_freemode_01"_joaat && model != "mp_f_freemode_01"_joaat)
				model = "mp_m_freemode_01"_joaat;
		}

		src->set_return_value<Hash>(std::move(model));
	}
}
