#include "byte_patch.hpp"

#include "commands/patches/byte_patch_manager.hpp"

namespace memory
{
	byte_patch::~byte_patch()
	{
		restore();
	}

	void byte_patch::apply() const
	{
		std::lock_guard lock(g_patches_mutex);
		DWORD temp;

		VirtualProtect(m_address, m_size, PAGE_EXECUTE_READWRITE, (PDWORD)&m_old_protect);
		memcpy(m_address, m_value.get(), m_size);
		VirtualProtect(m_address, m_size, m_old_protect, &temp);
	}

	void byte_patch::restore() const
	{
		std::lock_guard lock(g_patches_mutex);
		DWORD temp;

		VirtualProtect(m_address, m_size, PAGE_EXECUTE_READWRITE, (PDWORD)&temp);
		memcpy(m_address, m_original_bytes.get(), m_size);
		VirtualProtect(m_address, m_size, m_old_protect, &temp);
	}

	void byte_patch::remove() const
	{
		std::erase_if(m_patches, [this](auto& el) {
			return el.get() == this;
		});
	}

	void byte_patch::restore_all()
	{
		for (auto& patch : m_patches)
		{
			patch->restore();
			patch.reset();
		}

		m_patches.clear();
	}

	bool operator==(const std::unique_ptr<byte_patch>& a, const byte_patch* b)
	{
		return a->m_address == b->m_address;
	}
}
