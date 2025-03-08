#include "commands/math.h"
#include "commands/types/looped_command.hpp"
#include "rage/gta/net_array.hpp"
#include "rage/util.h"
#include "util/globals.h"

constexpr char transition_states[][48] = {
	"TRANSITION_STATE_EMPTY",
	"Singleplayer Swoop Up", "Multiplayer Swoop Up", "Creator Swoop Up", "Pre-HUD Checks",
	"Wait HUD Exit", "Wait For Summon", "Singleplayer Swoop Down", "Multiplayer Swoop Down", "Cancel Joining",
	"Retry Loading", "Retry Loading Slot 1", "Retry Loading Slot 2", "Retry Loading Slot 3", "Retry Loading Slot 4",
	"Wait On Invite", "Prejoining Freemode Session Checks", "Look For Fresh Join Freemode",
	"Look To Join Another Session Freemode", "Confirm Freemode Session Joining", "Wait Join Freemode Session",
	"Creation Enter Session", "Pre-Freemode Launch Script", "Freemode Teamfull Check",
	"Start Freemode Launch Script", "Freemode Transition Create Player", "Is Freemode And Transition Ready",
	"Freemode Swoop Down", "Post Bink Video Warp", "Freemode Final Setup Player", "Move Freemode To Running State",
	"Freemode How To Terminate", "Start Creator Pre-Launch Script Check", "Start Creator Launch Script",
	"Creator Transition Create Player", "Is Creator And Transition Ready", "Creator Swoop Down",
	"Creator Final Setup Player", "Move Creator To Running State", "Prejoining Testbed Session Checks",
	"Look For Fresh Join Testbed", "Look For Fresh Host Testbed", "Look To Join Another Session Testbed",
	"Look To Host Session Testbed", "Confirm Testbed Session Joining", "Wait Join Testbed Session",
	"Start Testbed Launch Script", "Testbed Transition Create Player", "Is Testbed And Transition Ready",
	"Testbed Swoop Down", "Testbed Final Setup Player", "Move Testbed To Running State", "Testbed How To Terminate",
	"Quit Current Session Prompt", "Wait For Transition Session To Setup", "Terminate Singleplayer",
	"Wait Terminate Singleplayer", "Kick Terminate Session", "Terminate Session", "Wait Terminate Session",
	"Terminate Session And Hold", "Terminate Session And Move Into Holding State", "Team Swapping Checks",
	"Return To Singleplayer", "Wait For Singleplayer To Start", "Waiting For External Termination Call",
	"Terminate Maintransition", "Wait For Dirty Load Confirm", "DLC Intro Bink", "Spawn Into Personal Vehicle"
};

namespace commands
{
	class transition_state final : looped_command
	{
		using looped_command::looped_command;

		eTransitionState last_state = eTransitionState::None;
		float old_percent = NAN;

		void on_tick() override
		{
			const auto state = *scr_globals::transition_state.as<eTransitionState*>();
			if (state == eTransitionState::TerminateMaintransition)
				return;

			// When free mode script loaded remove loading screen.
			if (state == eTransitionState::WaitJoinFreemodeSession && DLC::GET_IS_LOADING_SCREEN_ACTIVE())
				SCRIPT::SHUTDOWN_LOADING_SCREEN();

			if ((last_state == state && state != eTransitionState::FreemodeTransitionCreatePlayer) || state == eTransitionState::None || state > eTransitionState::SpwanIntoPersonalVehicle)
				return;

			if (state != eTransitionState::FreemodeTransitionCreatePlayer && HUD::BUSYSPINNER_IS_ON())
				HUD::BUSYSPINNER_OFF();

			if (static_cast<int>(state) > 0 && static_cast<int>(std::size(transition_states)) > static_cast<int>(state))
			{
				if (rage::scrThread* free_mode; state == eTransitionState::FreemodeTransitionCreatePlayer && (free_mode = find_script_thread("freemode"_joaat), free_mode && free_mode->m_net_component))
				{
					int num_array_handlers{};
					int received_array_handlers{};
					while (const auto handler = g_pointers->g_get_host_array_handler_by_index((CGameScriptHandlerNetComponent*)free_mode->m_net_component, num_array_handlers++))
					{
						if (handler->m_flags & 1)
						{
							received_array_handlers++;
						}
					}

					if (num_array_handlers == 0)
						num_array_handlers = 1;

					auto percent = round(static_cast<float>(received_array_handlers) / static_cast<float>(num_array_handlers) * 100);
					if (!math::are_floats_equal(percent, old_percent))
					{
						HUD::BEGIN_TEXT_COMMAND_BUSYSPINNER_ON("STRING");
						HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(std::format("Wait For Host Broadcast Data: {}%", percent).c_str());
						HUD::END_TEXT_COMMAND_BUSYSPINNER_ON(5);
					}
					old_percent = percent;
				}
				else
				{
					old_percent = NAN;
					HUD::BEGIN_TEXT_COMMAND_BUSYSPINNER_ON("STRING");
					HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(transition_states[static_cast<int>(state)]);
					HUD::END_TEXT_COMMAND_BUSYSPINNER_ON(5);
				}
			}
			last_state = state;
		}
	};

	transition_state g_transition_state("Transition State", "transition_state", "Show the transition state");
}
