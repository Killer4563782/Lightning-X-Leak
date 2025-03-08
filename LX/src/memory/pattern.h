#pragma once

namespace memory
{
	class pattern
	{
	public:
		pattern(std::string_view ida_sig);

		pattern(const char* ida_sig) :
			pattern(std::string_view(ida_sig))
		{}

		std::vector<std::optional<uint8_t>> m_bytes;
	};
}
