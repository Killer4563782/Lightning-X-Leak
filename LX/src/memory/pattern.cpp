#include "pattern.h"

namespace memory
{
	inline std::optional<uint8_t> to_hex(const char c)
	{
		if (c >= '0' && c <= '9')
			return c - '0';

		if (c >= 'a' && c <= 'f')
			return c - 'a' + 10;

		if (c >= 'A' && c <= 'F')
			return c - 'A' + 10;

		return std::nullopt;
	}

	pattern::pattern(std::string_view ida_sig)
	{
		const auto size_minus_one = ida_sig.size() - 1;
		m_bytes.reserve(size_minus_one / 2);
		for (size_t i = 0; i < size_minus_one; ++i)
		{
			if (ida_sig[i] == ' ') 
				continue;

			if (ida_sig[i] != '?')
			{
				auto c1 = to_hex(ida_sig[i]);
				auto c2 = to_hex(ida_sig[++i]);
				if (c1 && c2)
				{
					m_bytes.emplace_back(static_cast<uint8_t>(*c1 * 0x10 + *c2));
				}
			}
			else
			{
				m_bytes.emplace_back();
			}
		}
	}
}
