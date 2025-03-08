#pragma once

namespace entity_preview
{
	namespace vehicle
	{
		void clean_up();
		void preview(Hash vehicle_hash, bool max = false);
	}

	namespace ped
	{
		void clean_up();
		void preview(Hash ped_hash);
	}
};
