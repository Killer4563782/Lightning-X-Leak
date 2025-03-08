#pragma once

namespace recovery
{
	bool get_prefix(std::string* out);
	void unlock_all();
	void max_stats();
	void receive_all_achievements();
	void set_rank(int rank);
	void unlock_gender_change();
	void clear_all_reports();
}