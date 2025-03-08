#include "protections_sub.h"
#include "gui/types/break_option.hpp"
#include "gui/types/function_option.hpp"
#include "gui/types/toggle_option.hpp"
#include "services/reactions/reactions.h"

namespace ui::submenus
{
	protection_submenu::protection_submenu(std::string name, std::string description, Reaction* reaction, bool allow_block) : submenu(std::move(name), std::move(description)),
		m_reaction(reaction),
		m_allow_block(allow_block)
	{}

	std::string protection_submenu::get_tags() const
	{
		std::vector<std::string> tags;

		if (m_reaction->m_log)
			tags.emplace_back("L");

		if (m_reaction->m_notify)
			tags.emplace_back("N");

		if (m_allow_block && m_reaction->m_block)
			tags.emplace_back("B");

		if (m_reaction->m_kick)
			tags.emplace_back("K");

		if (m_reaction->m_crash)
			tags.emplace_back("C");

		return tags.empty() ? "" : "[" + join(tags, ",") + "]";
	}

	void protection_submenu::draw(bool selected)
	{
		submenu::draw(selected);
		g_base -= g_options.m_size;

		drawing::text(
			g_options.m_font,
			get_tags(),
			{
				g_pos.x + g_width - 0.01f,
				g_base + g_options.m_size / 2.f - drawing::get_text_height(g_options.m_font, g_options.m_text_size) / 2.f
			},
			g_options.m_text_size,
			g_options.text_color(selected),
			eJustify::Right,
			-g_options.m_padding,
			g_width
		);

		g_base += g_options.m_size;
	}


	void protection_submenu::on_init()
	{
		this->add(toggle_option("Log", "Log to console", &m_reaction->m_log));
		this->add(toggle_option("Notify", "Creates a notification which you can see while youre in game", &m_reaction->m_notify));

		if (m_allow_block)
			this->add(toggle_option("Block", "Block the event", &m_reaction->m_block));

		this->add(toggle_option("Kick", "Kick the event sender", &m_reaction->m_kick));
		this->add(toggle_option("Crash", "Crash the event sender", &m_reaction->m_crash));
	}


	void protections_submenu::on_init()
	{
		this->add(function_option("Toggle All", "Toggles the block state of all protections", [this]
		{
			m_toggle_all_state ^= true;

			for (const auto option : m_options)
			{
				if (const auto protection = dynamic_cast<protection_submenu*>(option))
				{
					if (protection->m_allow_block)
						protection->m_reaction->m_block = m_toggle_all_state;
				}
			}
		}));
		this->add(protection_submenu("Crash", {}, &crash));
		this->add(protection_submenu("Kick", {}, &kick));
		this->add(protection_submenu("Report", "Bad reports from players", &report));
		this->add(protection_submenu("Cage", {}, &cage));
		this->add(protection_submenu("PTFX Spam", {}, &ptfx_spam));
		this->add(protection_submenu("Vote Kick", "This can not be blocked", &vote_kick, false));
		this->add(protection_submenu("Cash Spawn", "Occurs when a modder spawns cash. This will make you automatically report the modder if its not blocked", &cash_spawn));
		this->add(protection_submenu("Report My Self", {}, &report_my_self));
		this->add(protection_submenu("Ragdoll", {}, &ragdoll));
		this->add(protection_submenu("Clear Ped Tasks", {}, &clear_ped_tasks));
		this->add(protection_submenu("Request Control", {}, &request_control));
		this->add(protection_submenu("Give Weapon", {}, &give_weapon));
		this->add(protection_submenu("Remove Weapon", {}, &remove_weapon));
		this->add(protection_submenu("Particle Spam", {}, &particle_spam));
		this->add(protection_submenu("Sound Spam", {}, &sound_spam));
		this->add(protection_submenu("Explosion", "All explosions", &explosion));
		this->add(protection_submenu("Explosion Spam", {}, &explosion_spam));
		this->add(protection_submenu("Modded Explosion", "This protection is still work in progress", &modded_explosion));
		this->add(protection_submenu("Pickup", "All pickups", &pickup));
		this->add(protection_submenu("Turn Into Beast", {}, &turn_into_beast, false));
		this->add(protection_submenu("Remote Wanted Level", {}, &remote_wanted_level));

		this->add(break_option("Script Events"));
		this->add(protection_submenu("Remove Godmode", "Prevent other modders from removing your godmode", &rotate_cam));
		this->add(protection_submenu("Fake Deposit", {}, &fake_deposit));
		this->add(protection_submenu("Bounty", {}, &bounty));
		this->add(protection_submenu("Ceo Kick", {}, &ceo_kick));
		this->add(protection_submenu("Ceo Money", {}, &ceo_money_not_boss));
		this->add(protection_submenu("Clear Wanted Level", {}, &clear_wanted_level));
		this->add(protection_submenu("Give Wanted Level", {}, &alter_wanted_level));
		this->add(protection_submenu("Force Mission", {}, &force_mission));
		this->add(protection_submenu("Give Collectible", {}, &give_collectible));
		this->add(protection_submenu("GTA Banner", {}, &gta_banner));
		this->add(protection_submenu("MC Teleport", {}, &mc_teleport));
		this->add(protection_submenu("Remote Off Radar", {}, &remote_off_radar));
		this->add(protection_submenu("Send to Location", {}, &send_to_location));
		this->add(protection_submenu("Send to Cutscene", {}, &send_to_cutscene));
		this->add(protection_submenu("Freeze", {}, &freeze));
		this->add(protection_submenu("Spectate", {}, &spectate));
		this->add(protection_submenu("Apartment Invite Spam", {}, &apartment_invite));
		this->add(protection_submenu("Force Teleport", {}, &force_teleport));
		this->add(protection_submenu("Transaction Error", {}, &transaction_error));
		this->add(protection_submenu("Vehicle Kick", {}, &vehicle_kick));
		this->add(protection_submenu("Bail", {}, &network_bail));
		this->add(protection_submenu("Start Activity", {}, &start_activity));
		this->add(protection_submenu("Trigger Business Raid", {}, &trigger_business_raid));
		this->add(protection_submenu("Destroy Personal Vehicle", {}, &personal_vehicle_destroyed));
		this->add(protection_submenu("Stolen Personal Vehicle", {}, &personal_vehicle_stolen));
		this->add(protection_submenu("Kick from Interior", {}, &kick_from_interior));
		this->add(protection_submenu("Start Script", {}, &start_script));
		this->add(protection_submenu("Mugger Loop", {}, &mugger_loop));
		this->add(protection_submenu("Passive Mode Block", "Prevent Modders from blocking you to go into passive mode", &passive_mode_block));
		this->add(protection_submenu("Disable Driving Vehicle", {}, &disable_driving_vehicle));
		this->add(protection_submenu("Sender Mismatch", {}, &sender_mismatch));
		this->add(protection_submenu("Break Game", {}, &break_game));
	}

	protections_submenu g_protections_submenu("Protections", "Options for protections");
}
