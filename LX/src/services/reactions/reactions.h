#pragma once
#include "services/players/player_manager.hpp"

class Reaction
{
public:
	explicit Reaction(bool notify = true, bool log = true, bool crash = false, bool kick = false, bool block = true);
	// Returns m_block
	bool process(const player_ptr& sender, const std::string& text, bool dont_notify = false) const;
	bool process(CNetGamePlayer* sender, const std::string& text, bool dont_notify = false) const;

	bool m_notify;
	bool m_log;
	bool m_crash;
	bool m_kick;
	bool m_block;
};

extern Reaction report;
extern Reaction vote_kick;
extern Reaction kick;
extern Reaction crash;
extern Reaction cage;
extern Reaction ptfx_spam;
extern Reaction cash_spawn;
extern Reaction report_my_self;
extern Reaction ragdoll;
extern Reaction clear_ped_tasks;
extern Reaction request_control;
extern Reaction give_weapon;
extern Reaction remove_weapon;
extern Reaction particle_spam;
extern Reaction sound_spam;
extern Reaction explosion;
extern Reaction explosion_spam;
extern Reaction modded_explosion;
extern Reaction pickup;
extern Reaction turn_into_beast;
extern Reaction remote_wanted_level;
extern Reaction break_game;

// Script Events
extern Reaction rotate_cam;
extern Reaction fake_deposit;
extern Reaction bounty;
extern Reaction ceo_kick;
extern Reaction ceo_money_not_boss;
extern Reaction clear_wanted_level;
extern Reaction alter_wanted_level;
extern Reaction force_mission;
extern Reaction give_collectible;
extern Reaction gta_banner;
extern Reaction mc_teleport;
extern Reaction remote_off_radar;
extern Reaction send_to_location;
extern Reaction send_to_cutscene;
extern Reaction freeze;
extern Reaction spectate;
extern Reaction apartment_invite;
extern Reaction force_teleport;
extern Reaction transaction_error;
extern Reaction vehicle_kick;
extern Reaction network_bail;
extern Reaction start_activity;
extern Reaction trigger_business_raid;
extern Reaction personal_vehicle_stolen;
extern Reaction kick_from_interior;
extern Reaction start_script;
extern Reaction mugger_loop;
extern Reaction personal_vehicle_destroyed;
extern Reaction passive_mode_block;
extern Reaction disable_driving_vehicle;
extern Reaction sender_mismatch;
