#pragma once
#include "memory/pointers.h"

template<typename T>
class pool_iterator
{
public:
	T* pool        = nullptr;
	uint32_t index = 0;

	explicit pool_iterator(T* pool, int32_t index = 0)
	{
		this->pool  = pool;
		this->index = index;
	}

	pool_iterator& operator++()
	{
		for (index++; index < pool->m_size; index++)
		{
			if (pool->is_valid(index))
			{
				return *this;
			}
		}

		index = pool->m_size;
		return *this;
	}

	rage::CEntity* operator*()
	{
		auto addr = pool->get_address(index);
		return (rage::CEntity*)addr;
	}

	bool operator!=(const pool_iterator& other) const
	{
		return this->index != other.index;
	}
};

template<typename T>
class pool_utils
{
public:
	inline auto to_array()
	{
		std::vector<Entity> arr;
		for (auto entity : *static_cast<T*>(this))
		{
			if (entity)
				arr.push_back(g_pointers->g_pointerToHandle(entity));
		}

		return arr;
	}

	auto begin()
	{
		return ++pool_iterator<T>(static_cast<T*>(this), -1);
	}

	auto end()
	{
		return ++pool_iterator<T>(static_cast<T*>(this), static_cast<T*>(this)->m_size);
	}
};

class vehicle_pool : public pool_utils<vehicle_pool>
{
public:
	UINT64* m_pool_address;
	UINT32 m_size;
	char _Padding2[36];
	UINT32* m_bit_array;
	char _Padding3[40];
	UINT32 m_item_count;

	inline bool is_valid(UINT32 i)
	{
		return (m_bit_array[i >> 5] >> (i & 0x1F)) & 1;
	}

	inline UINT64 get_address(UINT32 i)
	{
		return m_pool_address[i];
	}
};

class generic_pool : public pool_utils<generic_pool>
{
public:
	UINT64 m_pool_address; // 0x0
	BYTE* m_bit_array;     // 0x8
	UINT32 m_size;         // 0x10
	UINT32 m_item_size;    // 0x14
	UINT32 m_pad[2];       // 0x18
	UINT32 m_item_count;   // 0x20

	inline bool is_valid(UINT32 i)
	{
		return mask(i) != 0;
	}

	inline UINT64 get_address(UINT32 i)
	{
		return mask(i) & (m_pool_address + (uint64_t)i * m_item_size);
	}

	inline int get_item_count()
	{
		return (4 * m_item_count) >> 2;
	}

private:
	inline long long mask(UINT32 i)
	{
		long long num1 = m_bit_array[i] & 0x80;
		return ~((num1 | -num1) >> 63);
	}
};
static_assert(offsetof(generic_pool, generic_pool::m_item_count) == 0x20);