#pragma once

namespace rage
{
	using joaat_t = uint32_t;

	constexpr char to_lower(const char c)
	{
		return c >= 'A' && c <= 'Z' ? c + ('a' - 'A') : c;
	}

	constexpr char to_upper(const char c)
	{
		return c >= 'a' && c <= 'z' ? c + ('A' - 'a') : c;
	}

	inline uint32_t joaat(const std::string_view str)
	{
		uint32_t hash = 0;
		for (const auto c : str)
			hash += to_lower(c), hash += hash << 10, hash ^= hash >> 6;
		hash += hash << 3, hash ^= hash >> 11, hash += hash << 15;
		return hash;
	}

	inline std::string jooat_to_char(uint32_t value)
	{
		std::string str;
		while (value != 0)
		{
			const char digit = static_cast<char>(value & 0xFF);
			str.insert(str.begin(), digit);
			value >>= 8;
		}
		return str;
	}
}

constexpr uint32_t constexpr_joaat(std::string_view str, const bool force_lower_case = true)
{
	uint32_t hash{};
	const char* s{ str.data() };
	while (*s != '\0')
	{
		hash += force_lower_case ? rage::to_lower(*s) : *s, ++s;
		hash += hash << 10, hash ^= hash >> 6;
	}
	hash += hash << 3, hash ^= hash >> 11, hash += hash << 15;

	return hash;
}

template <size_t ArraySize>
uint32_t joaat_from_char_array(const char (&str)[ArraySize])
{
	uint32_t hash{};
	for (int i{}; i != ArraySize; ++i)
	{
		hash += rage::to_lower(str[i]);
		hash += hash << 10, hash ^= hash >> 6;
	}
	hash += hash << 3, hash ^= hash >> 11, hash += hash << 15;

	return hash;
}

consteval uint32_t operator ""_joaat(const char* str, size_t) { return constexpr_joaat(str); }