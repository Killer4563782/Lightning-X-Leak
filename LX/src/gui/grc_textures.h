#pragma once

namespace rage
{
	enum class grcLockFlags : int32_t
	{
		Read = 1,
		Write = 2,
		Unknown = 4,
		WriteDiscard = 8,
		NoOverwrite = 16
	};

	class grcLockedTexture
	{
	public:
		int32_t m_level; //0x0000
		void* m_bits; //0x0004
		int32_t m_pitch; //0x000C
		int32_t m_pad; //0x0010
		uint32_t m_width; //0x0014
		uint32_t m_height; //0x0018
		int32_t m_format; //0x001C
		int32_t m_num_sub_levels; //0x0020
	}; //Size: 0x0028
	static_assert(sizeof(grcLockedTexture) == 0x28);

	struct grcTextureStored
	{
		char* m_name;
		ID3D11Resource* m_resource;
		ID3D11ShaderResourceView* m_shader;
		uint16_t m_width;
		uint16_t m_height;
		uint16_t m_depth;
	};

	class grcTexture
	{
	public:
		virtual ~grcTexture() = NULL;
		virtual bool unk_0008() = NULL;
		virtual int unk_0010() = NULL;
		virtual void unk_0018() = NULL;
		virtual int unk_0020() = NULL;
		virtual uint16_t GetWidth() = NULL;
		virtual uint16_t GetHeight() = NULL;
		virtual uint16_t GetDepth() = NULL;
		virtual uint8_t GetLevels() = NULL;
		virtual void unk_0048() = NULL;
		virtual bool unk_0050() = NULL;
		virtual void unk_0058(int64_t) = NULL;
		virtual void unk_0060(void*) = NULL;
		virtual void unk_0068(void*) = NULL;
		virtual void unk_0070() = NULL;
		virtual grcTexture* unk_0078() = NULL;
		virtual grcTexture* unk_0080() = NULL;
		virtual bool unk_0088() = NULL;
		virtual int unk_0090() = NULL;
		virtual int unk_0098() = NULL;
		virtual int unk_00A0() = NULL;
		virtual int unk_00A8() = NULL;
		virtual int unk_00B0() = NULL;
		virtual int unk_00B8() = NULL;
		virtual int unk_00C0() = NULL;
		virtual bool Map(int numSubLevels, int subLevel, grcLockedTexture* lockedTexture, grcLockFlags flags) = NULL;
		char pad_0008[32]; //0x0008
		char* m_name; //0x0028
		uint16_t m_ref_count; //0x0030
		uint8_t m_resource_type; //0x0032
		uint8_t m_layers; //0x0033
		char pad_0034[4]; //0x0034
		ID3D11Resource* m_resource; //0x0038
		uint32_t m_physical_size; //0x0040
		uint32_t m_handle; //0x0044
		uint32_t m_flags; //0x0048
		char pad_004C[4]; //0x004C
		uint16_t m_width; //0x0050
		uint16_t m_height; //0x0052
		uint16_t m_depth; //0x0054
		uint16_t m_mip_stride; //0x0056
		uint32_t m_format; //0x0058
		uint8_t m_image_type; //0x005C
		uint8_t m_mip_count; //0x005D
		uint8_t m_cut_mip_levels; //0x005E
		bool m_is_srgb; //0x005F
		grcTexture* m_previous; //0x0060
		grcTexture* m_next; //0x0068
		void* m_backing; //0x0070
		ID3D11ShaderResourceView* m_shader; //0x0078
		char pad_0080[40]; //0x0080
		void set_shader(ID3D11ShaderResourceView* shader, ID3D11Resource* resource = nullptr)
		{
			m_shader = shader;
			if (!resource)
			{
				m_shader->GetResource(&resource);
			}
			m_resource = resource;
		}

		void set_size(Vector2 size)
		{
			m_width = static_cast<uint16_t>(size.x);
			m_height = static_cast<uint16_t>(size.y);
		}

		Vector2 size() const
		{
			return {static_cast<float>(m_width), static_cast<float>(m_height)};
		}

		void Set(std::string file, ID3D11ShaderResourceView* shader, Vector2 size)
		{
			m_name = new char[file.length() + 1];
			//Yes, I know this is never deleted. The game will handle it, so hush...
			strcpy_s(m_name, file.length() + 1, file.c_str());
			set_shader(shader);
			set_size(size);
			m_depth = 1;
		}

		void Swap(grcTexture* texure, bool retainOrginialSizes = false)
		{
			m_resource = texure->m_resource;
			m_shader = texure->m_shader;
			if (!retainOrginialSizes)
			{
				m_width = texure->m_width;
				m_height = texure->m_height;
				m_depth = texure->m_depth;
			}
		}

		void Swap(grcTextureStored texure, bool retainOrginialSizes = false)
		{
			m_resource = texure.m_resource;
			m_shader = texure.m_shader;
			m_width = texure.m_width;
			m_height = texure.m_height;
			m_depth = texure.m_depth;
		}

		grcTextureStored create_copy()
		{
			grcTextureStored data;
			data.m_name = m_name;
			data.m_resource = m_resource;
			data.m_shader = m_shader;
			data.m_width = m_width;
			data.m_height = m_height;
			data.m_depth = m_depth;
			return data;
		}
	}; //Size: 0x00A8
	static_assert(sizeof(grcTexture) == 0xA8);

	class grcRenderTarget : public grcTexture
	{
	public:
	}; //Size: 0x00A8
	static_assert(sizeof(grcRenderTarget) == 0xA8);

	class grcTextureFactory
	{
	public:
		virtual ~grcTextureFactory() = default;
		virtual grcTexture* unk_0008() = NULL;
		virtual grcTexture* CreateManualTexture() = NULL;
		virtual grcTexture* unk_0018() = NULL;
		virtual grcTexture* CreateTexture(const char* name, int32_t id) = NULL;
		virtual void unk_0028() = NULL;
		virtual void unk_0030() = NULL;
		virtual int TranslateFormatToParamFormat(int32_t format) = NULL;
		virtual void unk_0040() = NULL;
		virtual void unk_0048() = NULL;
		virtual void unk_0050() = NULL;
		virtual void unk_0058() = NULL;
		virtual void unk_0060() = NULL;
		virtual void unk_0068() = NULL;
		virtual void unk_0070() = NULL;
		virtual void unk_0078() = NULL;
		virtual void unk_0080() = NULL;
		virtual void unk_0088() = NULL;
		virtual grcTexture* CreateFromNativeTexture(const char* name, ID3D11Resource* nativeResource, void* a3) = NULL;
		virtual void unk_00A0() = NULL;
		virtual void PushRenderTarget(void* a1, grcRenderTarget* target, void* a3, int a4, bool a5, bool a6) = NULL;
		virtual void PopRenderTarget(void* a1, grcRenderTarget* target) = NULL;

	public:
		char pad_0008[8]; //0x0008
	}; //Size: 0x0010
	static_assert(sizeof(grcTextureFactory) == 0x10);

	template <typename T>

	class pgDictionary2
	{
	public:
		virtual ~pgDictionary2() = default;
		uint64_t unk_0008; //0x0008
		uint64_t unk_0010; //0x0010
		uint32_t unk_0018; //0x0018
		uint32_t unk_001C; //0x001C
		uint64_t unk_0020; //0x0020
		int16_t m_count; //0x0028
		int16_t unk_002A; //0x002A
		char pad_002C[4]; //0x002C
		T** m_items; //0x0030
		uint32_t unk_0038; //0x0038
		bool add_element(T* element)
		{
			m_items[m_count] = element;
			m_count++;
			return m_items && m_items[m_count - 1] && is_element_valid(m_count - 1);
		}

		bool is_element_valid(int16_t index)
		{
		    return m_items && index < m_count && m_items[index];
		}

		T* get_element(int16_t index)
		{
		    return is_element_valid(index) ? m_items[index] : nullptr;
		}
	}; //Size: 0x003C
	template <typename T>
	class pgDictionaryPool
	{
	public:
		pgDictionary2<T>* m_dictionary; //0x0000
		uint32_t m_capacity; //0x0008
		uint32_t m_dictionary_hash; //0x000C
		char pad_0010[8]; //0x0010
	}; //Size: 0x0018
	class grcTextureStore
	{
	public:
		char pad_0000[56]; //0x0000
		pgDictionaryPool<grcTexture>* m_pool; //0x0038
		uint8_t* m_mask; //0x0040
		uint32_t m_count; //0x0048
		uint32_t m_size; //0x0052
	public:
		auto bitshift_mask(uint32_t index, uint8_t amount) { return m_mask[index] >> amount; }

		bool is_element_valid(const uint32_t index)
		{
			if (!m_mask)
				return false;
			if (index >= m_count)
				return false;
			return ~bitshift_mask(index, 7) & 1;
		}

		pgDictionaryPool<grcTexture>* get_element(const uint32_t index)
		{
			if (is_element_valid(index))
			{
				if (m_pool[index].m_dictionary)
				{
					return &m_pool[index];
				}
			}
			return nullptr;
		}

		pgDictionary2<grcTexture>* find_dictionary(const uint32_t hash)
		{
			for (decltype(m_count) i{m_count}; i; --i)
			{
				if (const auto element = get_element(i); element)
				{
					if (element->m_dictionary_hash == hash)
					{
						return element->m_dictionary;
					}
				}
			}
			return nullptr;
		}

		std::map<uint32_t, pgDictionary2<grcTexture>*> get_all_dictionaries()
		{
			std::map<uint32_t, pgDictionary2<grcTexture>*> elements{};
			for (decltype(m_count) i{m_count}; i; --i)
			{
				if (const auto element{get_element(i)}; element)
				{
					if (element->m_dictionary_hash)
					{
						elements.insert({element->m_dictionary_hash, element->m_dictionary});
					}
				}
			}
			return elements;
		}

		grcTexture* find_texture(const std::string& dictionary, const std::string& name)
		{
		    const uint32_t hash{joaat(dictionary)};
			const uint32_t texture_hash{joaat("platform:/textures/" + dictionary)}; 
		    for (const uint32_t current_hash : {hash, texture_hash})
		    {
		        if (const auto pg_dictionary{find_dictionary(current_hash)})
		        {
		            for (decltype(pg_dictionary->m_count) i{pg_dictionary->m_count}; i; --i)
		            {
		                if (const auto element{pg_dictionary->get_element(i)}; element && name == element->m_name)
		                {
		                    return element;
		                }
		            }
		        }
		    }

		    return nullptr;
		}
	}; //Size: 0x0056
}
