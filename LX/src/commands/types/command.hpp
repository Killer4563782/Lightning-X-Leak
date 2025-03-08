#pragma once
#include "command_arguments.hpp"
#include "fiber/fiber_pool.hpp"

namespace commands
{
	class command;
	inline std::unordered_map<std::string, command*> g_commands;

	class command
	{
	public:
		virtual ~command() = default;
		std::string m_name;
		std::string m_command;
		std::string m_description;
		std::optional<uint8_t> m_num_args;
		bool m_requires_fiber_pool;

		command(std::string name, std::string command, std::string description, std::optional<uint8_t> num_args = std::nullopt, bool requires_fiber_pool = false) :
			m_name(std::move(name)),
			m_command(std::move(command)),
			m_description(std::move(description)),
			m_num_args(num_args),
			m_requires_fiber_pool(requires_fiber_pool)
		{
			if (!m_command.empty())
			{
				if (g_commands.contains(m_command))
				{
					unsigned i = 1;
					while (g_commands.contains(m_command + std::to_string(i)))
						i++;

					m_command += std::to_string(i);
				}

				g_commands[m_command] = this;
			}
		}


		virtual void execute(const command_arguments& args) = 0;
		virtual std::optional<command_arguments> parse_args(const std::vector<std::string>& args)
		{
			return { 0 };
		}

		void call(const command_arguments& args)
		{
			if (m_num_args.has_value() && args.size() < m_num_args.value())
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
				g_fiber_pool->queue_job([this, args] {
					execute(args);
				});
			}
			else execute(args);
		}

		void call(const std::vector<std::string>& args)
		{
			if (m_num_args.has_value() && args.size() < m_num_args.value())
			{
				notify(Warning, 3s, "Command '{}' called with the wrong number of arguments. Expected {} >= {}",
					m_name,
					args.size(),
					m_num_args.value()
				);

				return;
			}

			if (auto parsed = parse_args(args); parsed.has_value())
				call(parsed.value());
		}


		static command* get(const std::string& command)
		{
			if (g_commands.contains(command))
				return g_commands[command];

			return nullptr;
		}

		static command* find(const std::string& command)
		{
			if (auto direct = get(command))
				return direct;

			auto commands = std::views::filter(g_commands, [&command](auto& pair) {
				return pair.first.starts_with(command); // return pair.first.contains(command);
			});
			if (commands.empty())
				return nullptr;

			int diff = INT_MAX;
			commands::command* found_cmd = nullptr;
			for (auto& [name, cmd] : commands)
			{
				if (auto cmp = abs(name.compare(command)); cmp < diff)
				{
					diff = cmp;
					found_cmd = cmd;
				}
			}

			return found_cmd;
		}

		static void call(const std::string& command, const command_arguments& args)
		{
			if (command.empty())
			{
				notify(Warning, 3s, "Command name can not be empty");
				return;
			}

			auto cmd = find(command);
			if (cmd == nullptr)
			{
				notify(Warning, 3s, "Command '{}' was not found", command);
				return;
			}

			cmd->call(args);
		}
	};
}