#include "script_functions.h"
#include "fiber/fiber_pool.hpp"
#include "fiber/script.hpp"
#include "rage/util.h"
#include "util/scripts.h"

script_function::script_function(const std::string& name, const rage::joaat_t script, const std::string& pattern, int32_t offset) :
    m_name(name),
    m_script(script),
    m_pattern(pattern),
    m_offset(offset),
    m_ip(0)
{
}

void script_function::populate_ip()
{
	if (m_ip == 0)
	{
		auto program = find_script_program(m_script);

		if (!program)
			return;

		auto location = scripts::get_code_location_by_pattern(program, m_pattern);

		if (!location)
			LOG(error, "Failed to find pattern {} in script {}", m_name, program->m_name);
		else
			LOG_DEBUG( "Found pattern {} in script {}", m_name, program->m_name);

		m_ip = location.value() + m_offset;
	}
}

void script_function::call(rage::scrThread* thread, rage::scrProgram* program, std::initializer_list<uint64_t> args)
{
	auto tls_ctx   = rage::tlsContext::get();
	auto stack     = reinterpret_cast<uint64_t*>(thread->m_stack);
	auto og_thread = tls_ctx->m_script_thread;

	tls_ctx->m_script_thread           = thread;
	tls_ctx->m_is_script_thread_active = true;

	rage::scrThreadContext ctx = thread->m_context;

	for (auto& arg : args)
		stack[ctx.m_stack_pointer++] = arg;

	stack[ctx.m_stack_pointer++] = 0;
	ctx.m_instruction_pointer    = m_ip;
	ctx.m_state                  = rage::eThreadState::idle;

	g_pointers->g_script_vm(stack, reinterpret_cast<int64_t**>(g_pointers->g_globals), program, &ctx);

	tls_ctx->m_script_thread           = og_thread;
	tls_ctx->m_is_script_thread_active = og_thread != nullptr;
}

void script_function::call_latent(rage::scrThread* thread, rage::scrProgram* program, std::initializer_list<uint64_t> args, bool& done)
{
	g_fiber_pool->queue_job([this, thread, program, args, &done] 
	{
		auto stack = reinterpret_cast<uint64_t*>(thread->m_stack);

		auto result = rage::eThreadState::idle;

		rage::scrThreadContext ctx = thread->m_context;

		for (auto& arg : args)
		{
			stack[ctx.m_stack_pointer++] = arg;
		}

		stack[ctx.m_stack_pointer++] = 0;
		ctx.m_instruction_pointer    = m_ip;
		ctx.m_state                  = rage::eThreadState::idle;

		while (result != rage::eThreadState::killed)
		{
			auto tls_ctx   = rage::tlsContext::get();
			auto og_thread = tls_ctx->m_script_thread;

			tls_ctx->m_script_thread           = thread;
			tls_ctx->m_is_script_thread_active = true;

			auto old_ctx      = thread->m_context;
			thread->m_context = ctx;
			result = g_pointers->g_script_vm(stack, reinterpret_cast<int64_t**>(g_pointers->g_globals), program, &thread->m_context);
			thread->m_context = old_ctx;

			tls_ctx->m_script_thread           = og_thread;
			tls_ctx->m_is_script_thread_active = og_thread != nullptr;

			script::get_current()->yield();
		}

		done = true;
	});
}

void script_function::static_call(std::initializer_list<uint64_t> args)
{
	populate_ip();

	auto thread = reinterpret_cast<rage::scrThread*>(new uint8_t[sizeof(rage::scrThread)]);
	memcpy(thread, rage::tlsContext::get()->m_script_thread, sizeof(rage::scrThread));

	void* stack                       = new uint64_t[25000];
	thread->m_stack                   = static_cast<rage::scrValue*>(stack);
	thread->m_context.m_stack_size    = 25000;
	thread->m_context.m_stack_pointer = 1;

	call(thread, find_script_program(m_script), args);

	delete[] stack;
	delete[] reinterpret_cast<uint8_t*>(thread); // without the cast it ends up calling the destructor which leads to some pretty funny crashes
}

void script_function::operator()(std::initializer_list<uint64_t> args)
{
	populate_ip();

	if (m_ip == 0) return;

	auto thread  = find_script_thread(m_script);
	auto program = find_script_program(m_script);

	if (thread && program)
	{
		call(thread, program, args);
	}
}