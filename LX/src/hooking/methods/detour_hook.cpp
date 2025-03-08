#include "detour_hook.hpp"

#include "memory/mem.h"
#include "minhook/MinHook.h"

detour_hook::detour_hook(): m_original(nullptr), m_target(nullptr), m_detour(nullptr)
{
}

detour_hook::detour_hook(const std::string& name, void* detour)
{
	set_instance(name, detour);
}

detour_hook::detour_hook(const std::string& name, void* target, void* detour)
{
	set_instance(name, target, detour);
}

void detour_hook::set_instance(const std::string& name, void* detour)
{
	m_name = name;
	m_detour = detour;
}

void detour_hook::set_instance(const std::string& name, void* target, void* detour)
{
	m_name = name;
	m_target = target;
	m_detour = detour;

	create_hook();
}

void detour_hook::set_target_and_create_hook(void* target)
{
	m_target = target;
	create_hook();
}

void detour_hook::create_hook()
{
	if (!m_target)
		return;

	fix_hook_address();

	if (const auto status = MH_CreateHook(m_target, m_detour, &m_original); status != MH_OK)
	{
		LOG(error, "Failed to create hook '{}', error: {}", m_name.c_str(), MH_StatusToString(status));
	}
	//throw std::runtime_error(std::format("Failed to create hook '{}', error: {}", m_name, MH_StatusToString(status)));
}

detour_hook::~detour_hook() noexcept
{
	if (!m_target)
		return;

	MH_RemoveHook(m_target);
	//if (auto status = MH_RemoveHook(m_target); status != MH_OK)
		//LOG(error, "Failed to remove hook '{}', error: {}", m_name.c_str(), MH_StatusToString(status)); // BUG: Causes exception because logger is uninitialized
}

void detour_hook::enable()
{
	if (!m_target)
		return;

	if (const auto status = MH_QueueEnableHook(m_target); status != MH_OK)
	{
		LOG(error, "Failed to enable hook '{}', error: {}", m_name.c_str(), MH_StatusToString(status));
	}
	//throw std::runtime_error(std::format("Failed to enable hook 0x{:X} ({})", reinterpret_cast<uintptr_t>(m_target), MH_StatusToString(status)));
}

void detour_hook::disable() const
{
	if (!m_target)
		return;

	if (const auto status = MH_QueueDisableHook(m_target); status != MH_OK)
	{
		LOG(error, "Failed to disable hook '{}', error: {}", m_name.c_str(), MH_StatusToString(status));
	}
}

DWORD exp_handler(PEXCEPTION_POINTERS exp, std::string const& name)
{
	LOG(error, "Exception in '{}': 0x{:X}", name.c_str(), exp->ExceptionRecord->ExceptionCode);
	return exp->ExceptionRecord->ExceptionCode == STATUS_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH;
}

void detour_hook::fix_hook_address()
{
	auto ptr = mem(m_target);
	while (ptr.as<uint8_t&>() == 0xE9)
		ptr = ptr.call();

	m_target = ptr.as<void*>();
}