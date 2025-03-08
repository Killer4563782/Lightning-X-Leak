#pragma once
#include "commands/types/command.hpp"
#include "services/online/online.h"
#include "services/players/player_manager.hpp"

namespace commands
{
	class player_command : public command
	{
	public:
		player_command(const std::string& name, const std::string& command, const std::string& description, std::optional<uint8_t> num_args, bool requires_fiber_pool = false) :
			command(name, command, description, num_args.has_value() ? std::optional{ num_args.value() + 1 } : std::nullopt, requires_fiber_pool)
		{}


		virtual void execute(const command_arguments& args) override
		{
			command_arguments new_args(m_num_args.value(), args);

			if (!services::is_online())
				return;

			auto player_id = args.get<uint8_t>(0);
			if (auto self = g_player_manager->get_self(); self->id() == player_id)
			{
				if (m_requires_fiber_pool)
				{
					g_fiber_pool->queue_job([&] { execute(self, new_args); });
					return;
				}
				execute(self, new_args);
				return;
			}

			if (player_ptr player = g_player_manager->get_by_id(player_id))
			{
				if (m_requires_fiber_pool)
				{
					g_fiber_pool->queue_job([&] { execute(player, new_args); });
					return;
				}
				execute(player, new_args);
				return;
			}

			notify(Warning, 3s, "No player with the id '{}' was found", player_id);
		}
		virtual void execute(player_ptr player, const command_arguments& args) = 0;

		virtual std::optional<command_arguments> parse_args(const std::vector<std::string>& args) override
		{
			std::vector<std::string> new_args;
			command_arguments result(m_num_args.value());

			if (!services::is_online())
				return std::nullopt;

			if (args[0] == "me" || args[0] == "self" || to_lower(g_player_manager->get_self()->get_name()).starts_with(to_lower(args[0])))
			{
				if (auto self = g_player_manager->get_self())
					result.push(self->id());
			}
			else
			{
				int player_id = -1;
				for (auto& player : g_player_manager->players() | std::views::values)
				{
					if (player && to_lower(player->get_name()).starts_with(to_lower(args[0])))
					{
						player_id = player->id();
						break;
					}
				}

				if (player_id == -1)
				{
					notify(Warning, 3s, "No player with the name '{}' was found", args[0].c_str());
					return std::nullopt;
				}

				result.push(player_id);
			}

			new_args.reserve(args.size());
			std::copy(args.begin() + 1, args.end(), new_args.begin());
			// for (int i = 1; i < args.size(); i++)
			// 	new_args.push_back(args[i]);

			auto res = parse_args_p(new_args);
			if (!res.has_value())
				return std::nullopt;

			const auto& alt_args = res.value();
			for (size_t i = 0; i < alt_args.size(); ++i)
				result.push(alt_args.get(i));

			return result;
		}
		virtual std::optional<command_arguments> parse_args_p(const std::vector<std::string>& args)
		{
			return { 0 };
		}

		void call(const player_ptr& player, const command_arguments& args)
		{
			if (m_num_args.has_value() && args.size() < m_num_args.value() - 1)
			{
				notify(Warning, 3s, "Command '{}' called with the wrong number of arguments. Expected {} >= {}",
					m_name,
					args.size(),
					m_num_args.value()
				);
				return;
			}

			if (m_requires_fiber_pool)
			{
				g_fiber_pool->queue_job([this, player, args] {
					if (player->is_valid())
						execute(player, args);
				});
			}
			else
			{
				if (player->is_valid())
					execute(player, args);
			}
		}
	};
}
