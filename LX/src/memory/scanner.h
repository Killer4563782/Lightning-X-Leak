#pragma once
#include "mem.h"
#include "signature.hpp"

class pattern_batch
{
	struct ptr_entry
	{
		std::string name;
		signature pattern;
		std::function<void(mem)> callback;
	};
public:
	void add(const std::string& name, const std::string_view ptr, const std::function<void(mem)>& callback)
	{
		++m_total_sig_count;
		m_patterns.emplace_back(name, ptr, callback);
	}

	void run(std::string processname)
	{
		std::vector<std::future<void>> futures;
		futures.reserve(m_patterns.size());
		for (auto& [name, pattern, callback] : m_patterns)
		{
			futures.emplace_back(std::async(std::launch::async, [this, name, pattern, callback, processname]
			{
				hmodule mod(processname);
				if (auto res = pattern.get(mod))
				{
					callback(res);
					LOG_DEBUG("Found {} at {}+0x{:X}", name, mod.name(), res.as<uint64_t>() - mod.begin().as<uint64_t>());
					++this->m_found_sig_count;
				}
				else
				{
					LOG(error, "Failed to find signature {}[module = {}]", name, mod.name());
					++this->m_failed_sig_count;
				}
			}));
		}

		for (auto& future : futures)
		{
			future.wait();
		}

		m_patterns.clear();
	}

	std::atomic<uint16_t> m_total_sig_count;
	std::atomic<uint16_t> m_found_sig_count;
	std::atomic<uint16_t> m_failed_sig_count;
private:
	std::vector<ptr_entry> m_patterns;
};