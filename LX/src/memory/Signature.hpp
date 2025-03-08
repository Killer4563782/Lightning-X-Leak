#pragma once 
#include "mem.h"
#include "module.h"

class signature {
public:
	signature(const std::string_view ptr)
	{
		raw_pattern_string = ptr;

		m_bytes = create_bytes_from_string(ptr);

		return;
	}

	signature(const void* bytes, const std::string_view mask)
	{
		const auto size = mask.size();
		for (size_t i{}; i != size; ++i)
		{
			if (mask[i] != '?')
			{
				m_bytes.emplace_back(static_cast<const uint8_t*>(bytes)[i]);
			}
			else
			{
				m_bytes.emplace_back(std::nullopt);
			}
		}
	}

	signature(const char* ida_sig) : signature(std::string_view(ida_sig))
	{
		raw_pattern_string = ida_sig;
	}

	mem get(hmodule mod = {}) const
	{
		size_t max_shift = m_bytes.size();
		size_t max_idx = max_shift - 1;

		//Get wildcard index, and store max shiftable byte count
		size_t wild_card_idx{ static_cast<size_t>(-1) };
		for (int i{ static_cast<int>(max_idx - 1) }; i >= 0; --i)
		{
			if (!m_bytes.at(i).has_value())
			{
				max_shift = max_idx - i;
				wild_card_idx = i;
				break;
			}
		}

		//Store max shiftable bytes for non wildcards.
		size_t shift_table[UINT8_MAX + 1]{};
		for (size_t i{}; i <= UINT8_MAX; ++i)
		{
			shift_table[i] = max_shift;
		}

		//Fill shift table with sig bytes
		for (size_t i{ wild_card_idx + 1 }; i != max_idx; ++i)
		{
			shift_table[*m_bytes[i]] = max_idx - i;
		}

		//Loop data
		const auto scan_end = mod.begin() - m_bytes.size();
		for (size_t current_idx{}; current_idx <= scan_end;)
		{
			for (std::ptrdiff_t sig_idx{ static_cast<std::ptrdiff_t>(max_idx) }; sig_idx >= 0; --sig_idx)
			{
				if (m_bytes[sig_idx] && *mod.begin().add(current_idx + sig_idx).as<uint8_t*>() != *m_bytes[sig_idx])
				{
					current_idx += shift_table[*mod.begin().add(current_idx + max_idx).as<uint8_t*>()];
					break;
				}
				if (sig_idx == NULL)
				{
					return mod.begin().add(current_idx);
				}
			}
		}

		return {};
	}

	mem get_bruteforce(const hmodule& mod = {})
	{
		return find_pattern_bruteforce(m_bytes.data(), m_bytes.size(), mod);
	}

	std::vector<mem> get_all(hmodule mod = {})
	{
		std::vector<mem> results{};
		for (uintptr_t i{}; i < mod.size() - m_bytes.size(); ++i)
		{
			if (does_memory_match(mod.begin().add(i).as<uint8_t*>(), m_bytes.data(), m_bytes.size()))
			{
				results.emplace_back(mod.begin().as<char*>() + i);
			}
		}
		return results;
	}

	std::vector<std::optional<uint8_t>> m_bytes;
	std::string raw_pattern_string;
private:
	static std::vector<std::optional<uint8_t>> create_bytes_from_string(const std::string_view ptr)
	{
		std::vector<std::optional<uint8_t>> bytes;
		bytes.reserve(ptr.size() / 2); // Reserve space for efficiency

		for (size_t i = 0; i < ptr.size(); ++i)
		{
			if (ptr[i] == ' ')
			{
				continue;
			}
			if (ptr[i] == '?')
			{
				bytes.emplace_back(std::nullopt);
			}
			else if (i != ptr.size() - 1 && isxdigit(ptr[i]) && isxdigit(ptr[i + 1]))
			{
				auto hex_str = ptr.substr(i, 2);
				bytes.emplace_back(static_cast<uint8_t>(std::stoi(hex_str.data(), nullptr, 16)));
				++i; // Skip the next character since it's part of the current byte
			}
		}

		return bytes;
	}

	static bool does_memory_match(const uint8_t* target, const std::optional<uint8_t>* sig, const size_t len)
	{
		for (size_t i = 0; i < len; ++i)
		{
			if (sig[i] && *sig[i] != target[i])
			{
				return false;
			}
		}
		return true;
	}

	static char* find_pattern_bruteforce(const std::optional<uint8_t>* elements, const size_t b_count, hmodule mod = {})
	{
		for (uintptr_t i{}; i < mod.size() - b_count; ++i)
		{
			if (does_memory_match(mod.begin().add(i).as<uint8_t*>(), elements, b_count))
			{
				return mod.begin().as<char*>() + i;
			}
		}
		return nullptr;
	}
};