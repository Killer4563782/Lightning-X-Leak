#include "reactions.h"

Reaction::Reaction(bool notify, bool log, bool crash, bool kick, bool block) :
	m_notify(notify),
	m_log(log),
	m_crash(crash),
	m_kick(kick),
	m_block(block)
{}

bool Reaction::process(const player_ptr& sender, const std::string& text, bool dont_notify) const
{
	const std::string log_msg = (m_block ? "Prevented " : "") + text + (sender && sender->m_net_game_player ? std::string(" from ") + sender->get_name() : "");

	if (m_notify && !dont_notify)
		notify(Warning, 5s, log_msg);

	if (m_log && !dont_notify)
		LOG(warn, "[Protection] " + log_msg);

	if (m_crash)
	{
		// TODO: Add crash here
	}

	if (m_kick)
	{
		// TODO: Add kick here
	}

	return m_block;
}

bool Reaction::process(CNetGamePlayer* sender, const std::string& text, bool dont_notify) const
{
	if (sender)
		return process(g_player_manager->get_by_id(sender->m_player_id), text, dont_notify);

	return process(static_cast<const player_ptr&>(nullptr), text, dont_notify);
}

Reaction report{ };
Reaction vote_kick{ };
Reaction kick{ };
Reaction crash{ };
Reaction cage{ };
Reaction ptfx_spam{ };
Reaction cash_spawn{ true, true, false, false, false };
Reaction report_my_self{ };
Reaction ragdoll{ };
Reaction clear_ped_tasks{ };
Reaction request_control{ };
Reaction give_weapon{ };
Reaction remove_weapon{ };
Reaction particle_spam{ };
Reaction sound_spam{ };
Reaction explosion{ false, false, false, false, false };
Reaction explosion_spam{ };
Reaction modded_explosion{ };
Reaction pickup{ false, false, false, false, false };
Reaction turn_into_beast{ };
Reaction remote_wanted_level{ };
Reaction break_game{ };
	
// Script Events
Reaction rotate_cam{ };
Reaction fake_deposit{ };
Reaction bounty{ };
Reaction ceo_kick{ };
Reaction ceo_money_not_boss{ };
Reaction clear_wanted_level{ };
Reaction alter_wanted_level{ };
Reaction force_mission{ };
Reaction give_collectible{ };
Reaction gta_banner{ };
Reaction mc_teleport{ };
Reaction remote_off_radar{ };
Reaction send_to_location{ };
Reaction send_to_cutscene{ };
Reaction freeze{ };
Reaction spectate{ };
Reaction apartment_invite{ };
Reaction force_teleport{ };
Reaction transaction_error{ };
Reaction vehicle_kick{ };
Reaction network_bail{ };
Reaction start_activity{ };
Reaction trigger_business_raid{ };
Reaction personal_vehicle_stolen{ };
Reaction kick_from_interior{ };
Reaction start_script{ };
Reaction sender_mismatch{ };
Reaction mugger_loop{ };
Reaction personal_vehicle_destroyed{ };
Reaction passive_mode_block{ };
Reaction disable_driving_vehicle{ };