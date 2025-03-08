#include "stack_trace.h"

#include "commands/settings.h"
#include "memory/pointers.h"

namespace exceptions::stack_trace
{
	std::string create_stack_trace(EXCEPTION_POINTERS* exception_info)
	{
		constexpr DWORD msvc_exception_code = 0xe06d7363;
		std::lock_guard lock(mutex);
		std::stringstream stream;

		stream << "\n################################################################\n";
		stream << "Information:\n";

#ifndef _DEBUG // Only required for release
		stream << "    Game Base Address: \t" << HEX_TO_UPPER(reinterpret_cast<uint64_t>(GetModuleHandle(nullptr))) << '\n';
		stream << "    Menu Base Address: \t" << HEX_TO_UPPER(reinterpret_cast<uint64_t>(g_module)) << '\n';
#endif

		stream << "    Menu Version: \t" MENU_VERSION " : " SUPPORTED_VERSION "\n";
		stream << "    Game Version: \t" << (g_pointers->g_game_version ? g_pointers->g_game_version : "UNKNOWN") << '\n';
		stream << "    Exception Type: \t" << exception_code_to_string(exception_info->ExceptionRecord->ExceptionCode) << " " << exception_info->ExceptionRecord->ExceptionCode << '\n';

		if (auto msg = exception_to_message(exception_info); !msg.empty())
			stream << "    Exception Message: \t" << msg << '\n';

		if (exception_info->ExceptionRecord->ExceptionCode == msvc_exception_code)
			stream << "    C++ Exception: \t" << reinterpret_cast<std::exception*>(exception_info->ExceptionRecord->ExceptionInformation[1])->what() << '\n';
		
#ifdef _DEBUG
		if (commands::settings::debug::in_script_vm)
		{
			stream << "    Current Script: \t" << rage::tlsContext::get()->m_script_thread->m_name << '\n';
			stream << "    Thread Program Counter (Could be Inaccurate): " << exception_info->ContextRecord->Rdi - exception_info->ContextRecord->Rsi << '\n';
		}
#endif

		auto context = exception_info->ContextRecord;
		stream << "\nRegisters:\n"
			<< "    RAX:  \t" << HEX_TO_UPPER(context->Rax) << '\n'
			<< "    RCX:  \t" << HEX_TO_UPPER(context->Rcx) << '\n'
			<< "    RDX:  \t" << HEX_TO_UPPER(context->Rdx) << '\n'
			<< "    RBX:  \t" << HEX_TO_UPPER(context->Rbx) << '\n'
			<< "    RSI:  \t" << HEX_TO_UPPER(context->Rsi) << '\n'
			<< "    RDI:  \t" << HEX_TO_UPPER(context->Rdi) << '\n'
			<< "    RSP:  \t" << HEX_TO_UPPER(context->Rsp) << '\n'
			<< "    RBP:  \t" << HEX_TO_UPPER(context->Rbp) << '\n'
			<< "    R8:   \t" << HEX_TO_UPPER(context->R8) << '\n'
			<< "    R9:   \t" << HEX_TO_UPPER(context->R9) << '\n'
			<< "    R10:  \t" << HEX_TO_UPPER(context->R10) << '\n'
			<< "    R11:  \t" << HEX_TO_UPPER(context->R11) << '\n'
			<< "    R12:  \t" << HEX_TO_UPPER(context->R12) << '\n'
			<< "    R13:  \t" << HEX_TO_UPPER(context->R13) << '\n'
			<< "    R14:  \t" << HEX_TO_UPPER(context->R14) << '\n'
			<< "    R15:  \t" << HEX_TO_UPPER(context->R15) << '\n';

		cache_modules_info();

		stream << "\nStacktrace:";
		stream << dump_stacktrace(exception_info);

		stream << "\n################################################################\n";
		return stream.str();
	}

	std::string exception_code_to_string(DWORD code)
	{
#define MAP_PAIR_STRINGIFY(x) \
	{                         \
		x, #x                 \
	}
		static std::map<DWORD, std::string> exceptions = { MAP_PAIR_STRINGIFY(EXCEPTION_ACCESS_VIOLATION), MAP_PAIR_STRINGIFY(EXCEPTION_ARRAY_BOUNDS_EXCEEDED), MAP_PAIR_STRINGIFY(EXCEPTION_DATATYPE_MISALIGNMENT), MAP_PAIR_STRINGIFY(EXCEPTION_FLT_DENORMAL_OPERAND), MAP_PAIR_STRINGIFY(EXCEPTION_FLT_DIVIDE_BY_ZERO), MAP_PAIR_STRINGIFY(EXCEPTION_FLT_INEXACT_RESULT), MAP_PAIR_STRINGIFY(EXCEPTION_FLT_INEXACT_RESULT), MAP_PAIR_STRINGIFY(EXCEPTION_FLT_INVALID_OPERATION), MAP_PAIR_STRINGIFY(EXCEPTION_FLT_OVERFLOW), MAP_PAIR_STRINGIFY(EXCEPTION_FLT_STACK_CHECK), MAP_PAIR_STRINGIFY(EXCEPTION_FLT_UNDERFLOW), MAP_PAIR_STRINGIFY(EXCEPTION_ILLEGAL_INSTRUCTION), MAP_PAIR_STRINGIFY(EXCEPTION_IN_PAGE_ERROR), MAP_PAIR_STRINGIFY(EXCEPTION_INT_DIVIDE_BY_ZERO), MAP_PAIR_STRINGIFY(EXCEPTION_INT_OVERFLOW), MAP_PAIR_STRINGIFY(EXCEPTION_INVALID_DISPOSITION), MAP_PAIR_STRINGIFY(EXCEPTION_NONCONTINUABLE_EXCEPTION), MAP_PAIR_STRINGIFY(EXCEPTION_PRIV_INSTRUCTION), MAP_PAIR_STRINGIFY(EXCEPTION_STACK_OVERFLOW), MAP_PAIR_STRINGIFY(EXCEPTION_BREAKPOINT), MAP_PAIR_STRINGIFY(EXCEPTION_SINGLE_STEP) };
#undef MAP_PAIR_STRINGIFY

		if (const auto& it = exceptions.find(code); it != exceptions.end())
			return it->second;

		return "UNKNOWN";
	}

	void cache_modules_info()
	{
		PPEB peb = NtCurrentTeb()->ProcessEnvironmentBlock;
		if (!peb)
			return;

		auto ldr_data = peb->Ldr;
		if (!ldr_data)
			return;

		HMODULE hMods[1024];
		DWORD cbNeeded;
		DWORD module_count = 0;
		if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods), &cbNeeded)) {
			module_count = cbNeeded / sizeof(HMODULE);
		}

		if (module_count == modules.size())
			return;

		modules.clear();
		modules.reserve(module_count);
		auto module_list = &ldr_data->InMemoryOrderModuleList;
		auto module_entry = module_list->Flink;
		for (; module_list != module_entry; module_entry = module_entry->Flink)
		{
			auto table_entry = CONTAINING_RECORD(module_entry, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
			if (!table_entry)
				continue;

			if (table_entry->FullDllName.Buffer)
			{
				auto mod_info = module_info(table_entry->FullDllName.Buffer, table_entry->DllBase);

				if (mod_info.m_path == g_module_path)
					mod_info.m_path = SHORT_UPPER_BRAND ".dll";

				modules.emplace_back(std::move(mod_info));
			}
		}
	}

	const char* get_cpp_exception(EXCEPTION_POINTERS* exception_info)
	{
		constexpr DWORD msvc_exception_code = 0xe06d7363;
		if (exception_info->ExceptionRecord->ExceptionCode == msvc_exception_code)
		{
			return reinterpret_cast<const std::exception*>(exception_info->ExceptionRecord->ExceptionInformation[1])->what();
		}

		return nullptr;
	}

	std::vector<uint64_t> grab_stacktrace(EXCEPTION_POINTERS* exception_info)
	{
		std::vector<uint64_t> frame_pointers(32);
		CONTEXT context = *exception_info->ContextRecord;

		STACKFRAME64 frame{};
		frame.AddrPC.Mode = AddrModeFlat;
		frame.AddrFrame.Mode = AddrModeFlat;
		frame.AddrStack.Mode = AddrModeFlat;
		frame.AddrPC.Offset = context.Rip;
		frame.AddrFrame.Offset = context.Rbp;
		frame.AddrStack.Offset = context.Rsp;

		for (unsigned long long& m_frame_pointer : frame_pointers)
		{
			if (!StackWalk64(IMAGE_FILE_MACHINE_AMD64, GetCurrentProcess(), GetCurrentThread(), &frame, &context, nullptr, SymFunctionTableAccess64, SymGetModuleBase64, nullptr))
				break;

			m_frame_pointer = frame.AddrPC.Offset;
		}

		return frame_pointers;
	}

	module_info* get_module_by_address(uint64_t addr)
	{
		for (auto& mod_info : modules)
		{
			if (mod_info.m_base < addr && addr < mod_info.m_base + mod_info.m_size)
			{
				if (mod_info.m_path == g_module_path)
					mod_info.m_path = SHORT_UPPER_BRAND ".dll";

				return &mod_info;
			}
		}
		return nullptr;
	}

	module_info* load_or_get_module(const std::string& moduleName)	
	{
		HMODULE moduleHandle = GetModuleHandle(moduleName.c_str());
		if (!moduleHandle)
		{
			moduleHandle = LoadLibrary(moduleName.c_str());
			if (!moduleHandle)
			{
				return nullptr;
			}
		}

		uint64_t baseAddress = reinterpret_cast<uint64_t>(moduleHandle);

		return get_module_by_address(baseAddress);
	}

	std::string dump_stacktrace(EXCEPTION_POINTERS* exception_info)
	{
		std::stringstream stream;
		auto frame_pointers = grab_stacktrace(exception_info);

		char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME];
		auto symbol = reinterpret_cast<SYMBOL_INFO*>(buffer);
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		symbol->MaxNameLen = MAX_SYM_NAME;

		DWORD64 displacement64;
		DWORD displacement;
		IMAGEHLP_LINE64 line;
		line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

		for (size_t i = 0; i < frame_pointers.size() && frame_pointers[i]; ++i)
		{
			auto address = frame_pointers[i];

			stream << "\n    ";
			if (i + 1 >= 10)
			{
				int amount = (i + 1 >= 1000) ? 3 : static_cast<int>(std::to_string(i + 1).size() - 1);
				stream.seekp(-amount, std::stringstream::cur);
			}

			stream << "[" << i + 1 << "]\t\t";
			if (SymFromAddr(GetCurrentProcess(), address, &displacement64, symbol))
			{
				if (SymGetLineFromAddr64(GetCurrentProcess(), address, &displacement, &line))
				{
					stream << line.FileName << ":" << line.LineNumber << " " << std::string_view(symbol->Name, symbol->NameLen);
				}
				else if (auto module_info = get_module_by_address(address))
				{
					auto module_name = module_info->m_path.filename().string();
					if (module_name == g_module_path.filename())
						module_name = SHORT_UPPER_BRAND ".dll";

					if (module_info->m_base == reinterpret_cast<uint64_t>(GetModuleHandle(nullptr)))
					{
						stream << module_name << " " << std::string_view(symbol->Name, symbol->NameLen) << " (" << module_name << "+" << HEX_TO_UPPER(address - module_info->m_base) << ")";
					}
					else stream << module_name << " " << std::string_view(symbol->Name, symbol->NameLen);
				}
				else stream << std::string_view(symbol->Name, symbol->NameLen);
			}

			else if (auto module_info = get_module_by_address(address))
			{
				auto address2 = address;
				auto offset = address - module_info->m_base;
#ifndef _DEBUG
				// Change Address and Offset
				if (module_info->m_path == SHORT_UPPER_BRAND ".dll")
				{
					address2 *= 4;
					address2 += 123'456'789;
					address2 *= 2;
					offset = address - module_info->m_base;
				}
#endif
				stream << module_info->m_path.filename().string() << "+" << HEX_TO_UPPER(offset) << " " << HEX_TO_UPPER(address2);
			}
			else stream << HEX_TO_UPPER(address);
		}

		return stream.str();
	}

	// https://learn.microsoft.com/en-us/windows/win32/api/winnt/lualocal-winnt-exception_record?redirectedfrom=MSDN&devlangs=cpp&f1url=%3FappId%3DDev17IDEF1%26l%3DEN-US%26k%3Dk(WINNT%252FPEXCEPTION_RECORD)%3Bk(PEXCEPTION_RECORD)%3Bk(DevLang-C%252B%252B)%3Bk(TargetOS-Windows)%26rd%3Dtrue
	std::string exception_to_message(EXCEPTION_POINTERS* exception_info)
	{
		DWORD code = exception_info->ExceptionRecord->ExceptionCode;
		ULONG_PTR info_1 = exception_info->ExceptionRecord->ExceptionInformation[0];
		ULONG_PTR info_2 = exception_info->ExceptionRecord->ExceptionInformation[1];
		ULONG_PTR info_3 = exception_info->ExceptionRecord->ExceptionInformation[2];

		std::stringstream exception_message;
		switch (code)
		{
			case EXCEPTION_ACCESS_VIOLATION:
			case EXCEPTION_IN_PAGE_ERROR:
			switch (info_1)
			{
				case 0: exception_message << "Attempted to Read from Inaccessible Address: " << HEX_TO_UPPER(info_2); break;
				case 1: exception_message << "Attempted to Write to Inaccessible Address: " << HEX_TO_UPPER(info_2); break;
				case 8: exception_message << "Data Execution Prevention at: " << HEX_TO_UPPER(info_2); break;
				default:
				exception_message << (code == EXCEPTION_ACCESS_VIOLATION ? "Unknown Access Violation at: 0x" : "Unknown In Page Error at: 0x") << std::hex << info_2;
				break;
			}

			if (code == EXCEPTION_IN_PAGE_ERROR)
				exception_message << ", NTSTATUS: " << std::setw(8) << std::setfill('0') << HEX_TO_UPPER(info_3);
			break;

			case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			exception_message << "Tried to access an array element that is out of bounds and the underlying hardware supports bounds checking";
			break;

			case EXCEPTION_BREAKPOINT:
			exception_message << "A breakpoint was encountered";
			break;

			case EXCEPTION_DATATYPE_MISALIGNMENT:
			exception_message << "Tried to read or write data that is misaligned on hardware that does not provide alignment";
			break;

			case EXCEPTION_FLT_DENORMAL_OPERAND:
			exception_message << "One of the operands in a floating-point operation is denormal";
			break;

			case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			exception_message << "Tried to divide a floating-point value by a floating-point divisor of zero";
			break;

			case EXCEPTION_FLT_INEXACT_RESULT:
			exception_message << "The result of a floating-point operation cannot be represented exactly as a decimal fraction";
			break;

			case EXCEPTION_FLT_INVALID_OPERATION:
			exception_message << "An invalid floating-point operation was encountered";
			break;

			case EXCEPTION_FLT_OVERFLOW:
			exception_message << "The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type";
			break;

			case EXCEPTION_FLT_STACK_CHECK:
			exception_message << "The stack overflowed or underflowed as the result of a floating-point operation";
			break;

			case EXCEPTION_FLT_UNDERFLOW:
			exception_message << "The exponent of a floating-point operation is less than the magnitude allowed by the corresponding type";
			break;

			case EXCEPTION_ILLEGAL_INSTRUCTION:
			exception_message << "Tried to execute an invalid instruction";
			break;

			case EXCEPTION_INT_DIVIDE_BY_ZERO:
			exception_message << "Tried to divide an integer value by an integer divisor of zero";
			break;

			case EXCEPTION_INT_OVERFLOW:
			exception_message << "The result of an integer operation caused a carry out of the most significant bit of the result";
			break;

			case EXCEPTION_INVALID_DISPOSITION:
			exception_message << "An exception handler returned an invalid disposition to the exception dispatcher";
			break;

			case EXCEPTION_NONCONTINUABLE_EXCEPTION:
			exception_message << "Tried to continue execution after a noncontinuable exception occurred";
			break;

			case EXCEPTION_PRIV_INSTRUCTION:
			exception_message << "Tried to execute an instruction whose operation is not allowed in the current machine mode";
			break;

			case EXCEPTION_SINGLE_STEP:
			exception_message << "A trace trap or other single-instruction mechanism signaled that one instruction has been executed";
			break;

			case EXCEPTION_STACK_OVERFLOW:
			exception_message << "The thread used up its stack";
			break;
		}

		return exception_message.str();
	}
}
