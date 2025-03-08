#include <hooking/hooking.hpp>

void* hooks::SMPACreateStub(void* a1, void* a2, size_t size, void* a4, bool a5)
{
	if (size == 0xD00000)
	{
		rage::tlsContext::get()->m_allocator->Free(a2);

		size = 0x1200000;
		a2 = rage::tlsContext::get()->m_allocator->Allocate(size, 16, 0);
	}

	return  g_hooking->get_original<SMPACreateStub>()(a1,a2,size,a4,a5);
}