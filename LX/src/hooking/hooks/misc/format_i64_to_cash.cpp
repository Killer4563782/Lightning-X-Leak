#include "commands/manager.hpp"
#include "commands/settings.h"
#include "hooking/hooking.hpp"

void hooks::format_int64_to_cash(int64_t cash, char* cash_string, int cash_string_size, bool use_commas)
{
	// TODO: make it work for bank and wallet instead of both being the same
	if (commands::settings::misc::fake_money_enabled)
		cash = commands::settings::misc::fake_money;

	if ("money_seperator"_BC->m_toggle)
		use_commas = true;

	g_hooking->get_original<format_int64_to_cash>()(cash, cash_string, cash_string_size, use_commas);
}
