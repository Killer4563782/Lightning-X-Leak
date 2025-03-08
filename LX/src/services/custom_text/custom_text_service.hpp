#pragma once

using custom_label_callback = std::function<const char*(const char*)>;

class custom_text_service final
{
	std::map<rage::joaat_t, custom_label_callback> m_callbacks;
	std::map<rage::joaat_t, std::unique_ptr<char[]>> m_label_overwrites;

public:
	custom_text_service();
	~custom_text_service() = default;

	bool add_callback_for_label(rage::joaat_t hash, custom_label_callback&& cb);
	bool add_callback_for_labels(const std::list<rage::joaat_t>& hashes, custom_label_callback&& cb);
	bool add_label_overwrite(rage::joaat_t hash, std::string_view overwrite);

	[[nodiscard]] const char* get_text(const char* label) const;
};

inline std::unique_ptr<custom_text_service> g_custom_text_service;
