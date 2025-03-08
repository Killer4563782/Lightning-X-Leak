#pragma once

namespace exceptions::stack_trace
{
	struct module_info
	{
		module_info(const fs::path& path, void* base) : m_path(path), m_base(reinterpret_cast<uintptr_t>(base))
		{
			auto dos_header = static_cast<IMAGE_DOS_HEADER*>(base);
			auto nt_header = reinterpret_cast<IMAGE_NT_HEADERS*>(m_base + dos_header->e_lfanew);
			m_size = nt_header->OptionalHeader.SizeOfCode;
		}

		fs::path m_path;
		uintptr_t m_base;
		size_t m_size;
	};

	inline std::mutex mutex;
	inline std::vector<module_info> modules;

	std::string create_stack_trace(EXCEPTION_POINTERS* exception_info);
	module_info* load_or_get_module(const std::string& moduleName);
	std::string exception_code_to_string(DWORD code);
	void cache_modules_info();
	const char* get_cpp_exception(EXCEPTION_POINTERS* exception_info);
	std::vector<uint64_t> grab_stacktrace(EXCEPTION_POINTERS* exception_info);
	module_info* get_module_by_address(uint64_t addr);
	std::string dump_stacktrace(EXCEPTION_POINTERS* exception_info);
	std::string exception_to_message(EXCEPTION_POINTERS* exception_info);
}