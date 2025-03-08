#pragma once
#include "mem.h"

class hmodule {
public:
	hmodule(const std::string_view name = {}) : m_name(name), m_handle(get()), m_base(m_handle)
	{
		const auto dos_header = m_base.as<IMAGE_DOS_HEADER*>();
		const auto nt_header = m_base.add(dos_header->e_lfanew).as<IMAGE_NT_HEADERS*>();
		m_size = nt_header->OptionalHeader.SizeOfImage;
	}

	mem begin()
	{
		return m_base;
	}

	mem end()
	{
		return begin().add(size());
	}

	size_t size()
	{
		return m_size;
	}

	FARPROC get_process(const std::string& search)
	{
		return GetProcAddress(m_handle, search.c_str());
	}

	HMODULE get()
	{
		if (m_name.empty())
		{
			return GetModuleHandleA(nullptr);
		}
		return GetModuleHandleA(m_name.data());
	}

	const char* name()
	{
		return m_name.data();
	}
private:
	std::string_view m_name{};
	HMODULE m_handle{};
	mem m_base{};
	size_t m_size{};
};
