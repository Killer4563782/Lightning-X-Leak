#include "sessionStarterHelpers.h"
#include "commands/patches/script_functions.h"
#include "fiber/script.hpp"
#include "memory/pointers.h"
#include "services/online/online.h"
#include "util/globals.h"

namespace session_starter
{
	bool join_type(eSessionType session)
	{
		if (!SCRIPT::HAS_SCRIPT_WITH_NAME_HASH_LOADED("pausemenu_multiplayer"_joaat))
		{
			SCRIPT::REQUEST_SCRIPT_WITH_NAME_HASH("pausemenu_multiplayer"_joaat);

			while (!SCRIPT::HAS_SCRIPT_WITH_NAME_HASH_LOADED("pausemenu_multiplayer"_joaat))
				script::get_current()->yield();
		}

		// If SCTV then enable spectator mode
		*scr_globals::sctv_spectator.as<int*>() = session == eSessionType::SC_TV ? 1 : 0;

		if (session == eSessionType::LEAVE_ONLINE)
			*scr_globals::session.at(2).as<int*>() = -1;
		else
		{
			*scr_globals::session.at(2).as<int*>() = 0;
			*scr_globals::session2.as<int*>() = static_cast<int>(session);
		}

		*scr_globals::session.as<int*>() = 1;

		if (services::is_online() && session != eSessionType::LEAVE_ONLINE)
		{
			*scr_globals::transition_state.as<eTransitionState*>() = eTransitionState::LookToJoinAnotherSessionFreemode;
		}
		else if (session == eSessionType::LEAVE_ONLINE)
		{
			*scr_globals::transition_state.as<eTransitionState*>() = eTransitionState::ReturnToSingeplayer;
		}

		scr_functions::reset_session_data({true, true});
		*scr_globals::session3.as<int*>() = 0;
		*scr_globals::session4.as<int*>() = 1;
		*scr_globals::session5.as<int*>() = 32;

		if (SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("maintransition"_joaat) == 0)
		{
			*scr_globals::session6.as<int*>() = 1;
			script::get_current()->yield(200ms);
			*scr_globals::session.as<int*>() = 0;
		}

		//SCRIPT::SET_SCRIPT_WITH_NAME_HASH_AS_NO_LONGER_NEEDED("pausemenu_multiplayer"_joaat);
		return true;
	}
}
