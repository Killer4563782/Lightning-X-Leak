#include "hooking/hooking.hpp"
#include "rage/gta/net_game_event.hpp"
#include "services/reactions/reactions.h"

bool hooks::increment_stat_event(const CNetworkIncrementStatEvent* net_event, CNetGamePlayer* sender)
{
	switch (net_event->m_stat)
	{
		case "MPPLY_BAD_CREW_STATUS"_joaat:
		return report.process(sender, "Bad Crew Status Report");

		case "MPPLY_BAD_CREW_MOTTO"_joaat:
		return report.process(sender, "Bad Crew Motto Report");

		case "MPPLY_BAD_CREW_NAME"_joaat:
		return report.process(sender, "Bad Crew Name Report");

		case "MPPLY_BAD_CREW_EMBLEM"_joaat:
		return report.process(sender, "Bad Crew Emblem Report");

		case "MPPLY_EXPLOITS"_joaat:
		return report.process(sender, "Exploiting Report");

		case "MPPLY_GAME_EXPLOITS"_joaat:
		return report.process(sender, "Game Exploiting Report");

		case "MPPLY_TC_ANNOYINGME"_joaat:
		return report.process(sender, "Text Chat Annoying Me Report");

		case "MPPLY_TC_HATE"_joaat:
		return report.process(sender, "Text Chat Hate Report");

		case "MPPLY_VC_ANNOYINGME"_joaat:
		return report.process(sender, "Voice Chat Annoying Me Report");

		case "MPPLY_VC_HATE"_joaat:
		return report.process(sender, "Voice Chat Hate Report");
		default:;
	}

	return false;
}
