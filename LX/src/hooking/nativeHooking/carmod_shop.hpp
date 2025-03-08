#pragma once
#include "native_hooks.hpp"

namespace carmod_shop
{
	inline void STAT_GET_INT(rage::scrNativeCallContext* src)
	{
		const auto hash = src->get_arg<Hash>(0);
		auto out = src->get_arg<int*>(1);
		src->set_return_value<BOOL>(1);

		switch (hash)
		{
			case "MP0_RACES_WON"_joaat:
			case "MP0_NUMBER_TURBO_STARTS_IN_RACE"_joaat:
			case "MP0_USJS_COMPLETED"_joaat:
			case "MP0_AWD_FM_RACES_FASTEST_LAP"_joaat:
			case "MP1_RACES_WON"_joaat:
			case "MP1_NUMBER_TURBO_STARTS_IN_RACE"_joaat:
			case "MP1_USJS_COMPLETED"_joaat:
			case "MP1_AWD_FM_RACES_FASTEST_LAP"_joaat:
			*out = 50;
			break;
			case "MP0_NUMBER_SLIPSTREAMS_IN_RACE"_joaat:
			case "MP1_NUMBER_SLIPSTREAMS_IN_RACE"_joaat:
			*out = 100;
			break;
			case "MP0_CHAR_FM_CARMOD_1_UNLCK"_joaat:
			case "MP0_CHAR_FM_CARMOD_2_UNLCK"_joaat:
			case "MP0_CHAR_FM_CARMOD_3_UNLCK"_joaat:
			case "MP0_CHAR_FM_CARMOD_4_UNLCK"_joaat:
			case "MP0_CHAR_FM_CARMOD_5_UNLCK"_joaat:
			case "MP0_CHAR_FM_CARMOD_6_UNLCK"_joaat:
			case "MP0_CHAR_FM_CARMOD_7_UNLCK"_joaat:
			case "MP1_CHAR_FM_CARMOD_1_UNLCK"_joaat:
			case "MP1_CHAR_FM_CARMOD_2_UNLCK"_joaat:
			case "MP1_CHAR_FM_CARMOD_3_UNLCK"_joaat:
			case "MP1_CHAR_FM_CARMOD_4_UNLCK"_joaat:
			case "MP1_CHAR_FM_CARMOD_5_UNLCK"_joaat:
			case "MP1_CHAR_FM_CARMOD_6_UNLCK"_joaat:
			case "MP1_CHAR_FM_CARMOD_7_UNLCK"_joaat:
			*out = -1;
			break;
			case "MP0_AWD_FMRALLYWONDRIVE"_joaat:
			case "MP0_AWD_FMRALLYWONNAV"_joaat:
			case "MP0_AWD_FMWINSEARACE"_joaat:
			case "MP0_AWD_FMWINAIRRACE"_joaat:
			case "MP1_AWD_FMRALLYWONDRIVE"_joaat:
			case "MP1_AWD_FMRALLYWONNAV"_joaat:
			case "MP1_AWD_FMWINSEARACE"_joaat:
			case "MP1_AWD_FMWINAIRRACE"_joaat:
			*out = 1;
			break;
			default:
			src->set_return_value<BOOL>(STATS::STAT_GET_INT(hash, out, src->get_arg<int>(2)));
			break;
		}
	}

	inline void STAT_SET_INT(rage::scrNativeCallContext* src)
	{
		const auto hash = src->get_arg<Hash>(0);
		if (hash == "SP0_TOTAL_CASH"_joaat || hash == "SP1_TOTAL_CASH"_joaat || hash == "SP2_TOTAL_CASH"_joaat)
			return;

		src->set_return_value<BOOL>(STATS::STAT_SET_INT(hash, src->get_arg<int>(1), src->get_arg<int>(2)));
	}
}
