#include "custom_text_service.hpp"

custom_text_service::custom_text_service()
{
	add_label_overwrite("RESPAWN_W_MP"_joaat, "BRUH");
}

bool custom_text_service::add_callback_for_label(rage::joaat_t hash, custom_label_callback&& cb)
{
	return m_callbacks.insert({ hash, cb }).second;
}

bool custom_text_service::add_callback_for_labels(const std::list<rage::joaat_t>& hashes, custom_label_callback&& cb)
{
	bool result = true;
	for (const auto& hash : hashes)
		result = m_callbacks.insert({ hash, cb }).second;

	return result;
}

bool custom_text_service::add_label_overwrite(rage::joaat_t hash, const std::string_view overwrite)
{
	const auto size = std::strlen(overwrite.data()) + 1;
	auto buffer = std::make_unique<char[]>(size);
	memcpy(buffer.get(), overwrite.data(), size);

	return m_label_overwrites.insert({ hash, std::move(buffer) }).second;
}

const char* custom_text_service::get_text(const char* label) const
{
	const auto hash = rage::joaat(label);
	if (const auto& it = m_callbacks.find(hash); it != m_callbacks.end())
		return it->second(label);

	if (const auto& it = m_label_overwrites.find(hash); it != m_label_overwrites.end())
		return it->second.get();

	return nullptr;
}
