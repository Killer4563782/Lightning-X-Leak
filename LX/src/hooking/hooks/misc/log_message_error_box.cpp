#include "hooking/hooking.hpp"
#include "core/exception_handler/stack_trace.h"
#include "memory/pointers.h"

static int log_stack_trace_filter(EXCEPTION_POINTERS* ep)
{
	std::string st = exceptions::stack_trace::create_stack_trace(ep);
	LOG_DEBUG(st);

	return EXCEPTION_EXECUTE_HANDLER;
}

static std::string utf_16_to_code_page(uint32_t code_page, std::wstring_view input)
{
	if (input.empty())
		return {};

	auto size = WideCharToMultiByte(code_page, 0, input.data(), static_cast<int>(input.size()), nullptr, 0, nullptr, nullptr);

	std::string output(size, '\0');

	if (size != WideCharToMultiByte(code_page,
		0,
		input.data(),
		static_cast<int>(input.size()),
		output.data(),
		static_cast<int>(output.size()),
		nullptr,
		nullptr))
	{
		auto error_code = GetLastError();
		LOG_DEBUG("WideCharToMultiByte Error in String {}", error_code);
		return {};
	}

	return output;
}

static void log_stack_trace()
{
	__try
	{
		// On purpose to print a stack trace.
		int* ptr = nullptr;
		*ptr = 0;
	}
	__except (log_stack_trace_filter(GetExceptionInformation()))
	{
	}
}

void hooks::log_error_message_box(rage::joaat_t joaated_error_code, bool a2)
{
	if (joaated_error_code == "ERR_NET_EVENT"_joaat)
		return;

	LOG_DEBUG("Error Code: {}", utf_16_to_code_page(CP_UTF8, g_pointers->g_get_title_caption_error_message_box(joaated_error_code)).c_str());

	log_stack_trace();
}
