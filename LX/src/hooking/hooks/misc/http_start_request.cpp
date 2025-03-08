#include "hooking/hooking.hpp"

//vali: The request will never trigger if this returns false, much simpler hook that is easier to work with ; )
//vali: Also, you can block all telemetry. Metrics are fucking useless, they do dick all soooo (This will also block Bonus)
bool hooks::http_start_request(const char* uri)
{
	std::string str(uri);
	if (str.find("Telemetry") != -1)
	{
		return false;
	}

	return g_hooking->get_original<http_start_request>()(uri);
}
