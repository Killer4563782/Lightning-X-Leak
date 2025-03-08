#include "recovery.h"

#include "gui/types/function_option.hpp"
#include "helpers/inputs.h"
#include "helpers/recoveryHelpers.h"
#include "services/online/online.h"

namespace ui::submenus
{
	void recovery_submenu::on_init()
	{
		this->add(function_option("Unlock All", {}, recovery::unlock_all));
		this->add(function_option("Max Stats", {}, recovery::max_stats));
		this->add(function_option("Receive all Achievements", {}, recovery::receive_all_achievements));
		this->add(function_option("Set Rank", {}, []
		{
			if (!services::is_online())
				return notify(Error, 3s, "You need to be in online to use this");

			get_input("Enter Rank", "1 - 8000", text_input::InputFlags_Allow_Number, [](const std::string& input)
			{
				const auto rank = std::stoi(input);
				recovery::set_rank(rank);
			});
		}));
		this->add(function_option("Unlock Gender Change", "Unlock the gender change option in the interaction menu", recovery::unlock_gender_change));
		this->add(function_option("Clear all Reports", "Clear all reports of players who have reported you", recovery::clear_all_reports));
	}

	extern recovery_submenu g_recovery_submenu("Recovery", "No money :'(");
}
