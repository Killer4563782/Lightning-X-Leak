#pragma once
#include "gui/data.h"

namespace commands::settings
{
	namespace outfit_editor
	{
		inline int current_body_pos = 0;
		inline std::vector<std::string> body_pos_type = {
			"Head",
			"Torso",
			"Pants",
			"Shoes",
		};
	}

	namespace custom_lights
	{
		inline ui::color color = {0, 0, 0};
		inline float range = 1.f;
		inline float intensity = 1.f;
	}
	

	struct vfx
	{
		bool enable_custom_sky_color = false;

		ui::color azimuth_east = { 255, 0, 0, 255 };       // Initialized to red.
		ui::color azimuth_west = { 255, 0, 0, 255 };       // Initialized to red.
		ui::color azimuth_transition = { 255, 0, 0, 255 }; // Initialized to red.
		ui::color zenith = { 255, 0, 0, 255 };             // Initialized to red.

		float stars_intensity = 1.0f;
	};

	inline vfx g_vfx{};

	namespace time
	{
		inline int hours = 0;
		inline int minutes = 0;
		inline int seconds = 0;
	}

	namespace spoofing
	{
		inline bool spoof_godmode = true;
		inline bool spoof_spectate = true;
		inline bool override_game_hashes = false;
		inline std::string game_checksum_data_b64 = "";
		inline int last_game_version = -1;
		inline int game_dlc_checksum = -1;

	}

	namespace visuals
	{
		inline ui::color waypoint_color = {0, 255, 0};
	}

	namespace session
	{
		inline bool rejoin_desync_kick = false;
		inline bool prevent_network_bail = false;
		inline bool player_magnet = false;
		inline bool kick_host_to_stay_in_session = true;
		inline bool battleye_server = true;
	}

#ifdef _DEBUG
	namespace debug
	{
		inline bool in_script_vm = false;

		// Events
		inline bool log_events = false;
		inline bool log_script_events = false;

		// Protection Related
		inline bool log_nodes = false;
		inline bool log_clone_create = false;
		inline bool log_clone_sync = false;
		inline bool log_clone_remove = false;

		// Info
		inline bool log_metrics = false;
		inline bool log_packets = false;
		inline bool log_thread_start = false;
		inline bool log_thread_stop = false;
		
	}
#endif

	namespace misc
	{
		inline int64_t fake_money = 0;
		inline bool fake_money_enabled = false;
		inline bool join_as_sctv_without_spectating = false;
		inline bool run_commands_through_chat = false;
	}
	namespace lx_customs
	{
		inline float power = 1.f;
		inline float headlight = 1.f;
		inline int spoiler = 0;
		inline int frontbumper = 0;
		inline int rearbumper = 0;
		inline int sideskirt = 0;
		inline int exhaust = 0;
		inline int chassis = 0;
		inline int grille = 0;
		inline int hood = 0;
		inline int fender = 0;
		inline int rightfender = 0;
		inline int roof = 0;
		inline int engine = 0;
		inline int brakes = 0;
		inline int transmission = 0;
		inline int horns = 0;
		inline int supension = 0;
		inline int armor = 0;
		inline int turbo = 0;
		inline int livery = 0;
		inline int wheels = 0;
		inline int wheeltype = 0;
	}
}
