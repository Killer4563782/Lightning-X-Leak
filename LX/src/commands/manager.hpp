#pragma once
#include "patches/script_patches.hpp"
#include "fiber/script.hpp"
#include "rage/util.h"
#include "types/bool_command.hpp"
#include "types/command.hpp"
#include "types/looped_command.hpp"
#include "types/looped_vector_command.hpp"
#include "types/vector_command.hpp"
#include "types/number/float_command.hpp"
#include "types/number/int_command.hpp"
#include "types/number/looped_float_command.hpp"
#include "types/number/looped_int_command.hpp"
#include "types/player/player_command.hpp"

namespace commands::manager
{
	inline void execute(const std::string& string)
	{
		auto args = split(string, ' ');
		if (args.empty() || args[0].empty())
		{
			notify(Warning, 3s, "Can not run a empty command");
			return;
		}

		const auto& cmd_name = args[0];
		args.erase(args.begin());

		const auto cmd = command::find(cmd_name);
		if (!cmd)
		{
			notify(Warning, 3s, "Command not found");
			return;
		}

		cmd->call(args);
	}

	template <typename... T>
	void execute(const std::string& fmt, T... args)
	{
		const std::string string = std::vformat(fmt, std::make_format_args(args...));
		return execute(string);
	}

	inline bool display_loading_text = false;

	inline void system_update_desync_kick()
	{
		memset(&get_network()->m_game_complaint_mgr.m_host_tokens_complained, 0, 64 * sizeof(uint64_t));
		get_network()->m_game_complaint_mgr.m_num_tokens_complained = 0;

		for (const auto& player : g_player_manager->players() | std::views::values)
		{
			if (player->is_valid() && player->trigger_desync_kick)
			{
				get_network()->m_game_complaint_mgr.raise_complaint(player->get_net_data()->m_host_token);
			}
		}

		if (get_network()->m_game_session_state > 3 && get_network()->m_game_session_state < 6
			&& get_network()->m_game_complaint_mgr.m_num_tokens_complained && g_player_manager->get_self()->is_valid()
			&& !g_player_manager->get_self()->is_host())
		{
			g_pointers->g_reset_network_complaints(&get_network()->m_game_complaint_mgr, false);
		}
	}

	inline std::vector g_textures = {
		"commonmenu",
		"helicopterhud",
		"mprankbadge"
	};

	inline void init()
	{
		for (auto& texture : g_textures)
		{
			if (!GRAPHICS::HAS_STREAMED_TEXTURE_DICT_LOADED(texture))
			{
				GRAPHICS::REQUEST_STREAMED_TEXTURE_DICT(texture, TRUE);
				LOG(info, "Requesting Game Texture Directory: {}", texture);
				while (!GRAPHICS::HAS_STREAMED_TEXTURE_DICT_LOADED(texture))
				{
					script::get_current()->yield(350ms);
				}
			}
		}

		register_script_patches();

		while (g_running)
		{
			self::local_ped = *g_pointers->g_ped_factory ? (*g_pointers->g_ped_factory)->m_local_ped : nullptr;
			if (auto mgr = get_network_player_mgr(); mgr->m_local_net_player && mgr->m_local_net_player->m_player_id != static_cast<uint8_t>(-1))
			{
				self::local_player = mgr->m_local_net_player;
				self::player = mgr->m_local_net_player->m_player_id;
				if (self::player == 255)
				{
					LOG_DEBUG("Self Player ID is 255, This is not good!");
				}
			}
			else
			{
				self::local_player = nullptr;
				self::player = 0;
			}

			self::ped = PLAYER::PLAYER_PED_ID();
			self::position = ENTITY::GET_ENTITY_COORDS(self::ped, false);
			self::vehicle = PED::IS_PED_IN_ANY_VEHICLE(self::ped, 0) ? PED::GET_VEHICLE_PED_IS_IN(self::ped, false) : 0;

			if (g_open)
			{
				PAD::DISABLE_CONTROL_ACTION(0, ControlPhone, TRUE); // Disable phone
			}

			system_update_desync_kick();

			// Run looped commands
			for (auto command : g_looped_commands)
			{
				if (command->m_toggle)
					command->on_tick();
				command->refresh();
			}
			// Check if any bool commands have been changed
			for (auto command : g_bool_commands)
			{
				command->refresh();
			}

			// Run looped float commands
			for (auto command : g_looped_float_commands)
			{
				if (command->m_toggle)
					command->on_tick();
				command->refresh();
			}
			// Check if any float commands have been changed
			for (auto command : g_float_commands)
			{
				command->refresh();
			}

			// Run looped int commands
			for (auto command : g_looped_int_commands)
			{
				if (command->m_toggle)
					command->on_tick();
				command->refresh();
			}
			// Check if any int commands have been changed
			for (auto command : g_int_commands)
			{
				command->refresh();
			}

			// Run looped vector commands
			for (auto command : g_looped_vector_commands)
			{
				if (command->m_toggle)
					command->on_tick();
				command->refresh();
			}
			// Check if any vector commands have been changed
			for (auto command : g_vector_commands)
			{
				command->refresh();
			}

			script::get_current()->yield();
		}
	}

	inline void un_init()
	{
		for (const auto& command : g_bool_commands)
			command->on_reset();

		for (const auto& command : g_looped_commands)
			command->on_reset();


		for (const auto& command : g_looped_float_commands)
			command->on_reset();

		for (const auto& command : g_float_commands)
			command->on_reset();


		for (const auto& command : g_looped_int_commands)
			command->on_reset();

		for (const auto& command : g_int_commands)
			command->on_reset();


		for (const auto& command : g_looped_vector_commands)
			command->on_reset();

		for (const auto& command : g_vector_commands)
			command->on_reset();

		g_script_patcher_service->update();
		unregister_script_patches();
	}
}

template <typename T>
T* get_command(const char* str)
{
	return dynamic_cast<T*>(commands::command::find(str));
}

inline auto operator ""_C(const char* str, uint64_t)
{
	return get_command<commands::command>(str);
}

inline auto operator ""_BC(const char* command, uint64_t)
{
	return get_command<commands::bool_command>(command);

}
inline auto operator ""_LC(const char* command, uint64_t)
{
	return get_command<commands::looped_command>(command);
}

inline auto operator ""_FC(const char* command, uint64_t)
{
	return get_command<commands::float_command>(command);
}
inline auto operator ""_LFC(const char* command, uint64_t)
{
	return get_command<commands::looped_float_command>(command);
}

inline auto operator ""_IC(const char* command, uint64_t)
{
	return get_command<commands::int_command>(command);
}
inline auto operator ""_LIC(const char* command, uint64_t)
{
	return get_command<commands::looped_int_command>(command);
}

inline auto operator ""_VC(const char* command, uint64_t)
{
	return get_command<commands::vector_command>(command);
}
inline auto operator ""_LVC(const char* command, uint64_t)
{
	return get_command<commands::looped_vector_command>(command);
}

inline auto operator ""_PC(const char* command, uint64_t)
{
	return get_command<commands::player_command>(command);
}
