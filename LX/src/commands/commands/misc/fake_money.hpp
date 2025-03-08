#pragma once
#include "commands/manager.hpp"
#include "commands/settings.h"
#include "gui/types/function_option.hpp"
#include "gui/types/number_bool_option.hpp"
#include "gui/types/toggle_option.hpp"
#include "helpers/inputs.h"

namespace commands
{
	class fake_money_option : public number_bool_option<int64_t> 
	{
		using number_bool_option::number_bool_option;

		void action(ActionType type, bool first_receiver) override
		{
			switch (type)
			{
				case ActionType::Enter:
				{
					text_input::get_input("Type a number", "123456789...", text_input::InputFlags_Allow_Number | text_input::InputFlags_Dont_Add_History, std::to_string(*m_value), [this](const std::string& input)
					{
						if (input.empty())
						{
							*m_value = m_min;
							*m_toggle = false;
							return;
						}

						if (int64_t num; try_str_to_num(input, num))
						{
							*m_value = std::clamp(num, m_min, m_max);
							*m_toggle = true;
						}
						else
						{
							if (!std::ranges::all_of(input, [](char c)
							{
								return std::isdigit(c) || c == '-';
							}))
							
							return notify(Error, "The input was not a valid number");

							if (input[0] == '-')
							{
								*m_value = m_min;
							}
							else
							{
								*m_value = m_max;
							}

							*m_toggle = true;
						}
					});
					break;
				}
				case ActionType::Left:
				case ActionType::Right:
				{
					*m_toggle ^= 1;
					break;
				}
			}
		}
	};
}