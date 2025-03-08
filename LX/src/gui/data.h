#pragma once

#define GET_uint8_t(bitset, shift) (((bitset) >> (shift)) & 0xFF)

namespace ui
{
	class color
	{
	public:
		color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) :
			r(r),
			g(g),
			b(b),
			a(a)
		{}

		color(ImVec4 vec) : color(from_float(vec.x), from_float(vec.y), from_float(vec.z), from_float(vec.w))
		{}

		explicit color(const uint32_t packed) : color(GET_uint8_t(packed, 0), GET_uint8_t(packed, 8), GET_uint8_t(packed, 16), GET_uint8_t(packed, 24))
		{}

		color() = default;

		uint8_t r{};
		uint8_t g{};
		uint8_t b{};
		uint8_t a{};

		uint32_t pack() const
		{
			return a << 24 | b << 16 | g << 8 | r << 0;
		}

		float* float4() const
		{
			float value[4]{ to_float(a), to_float(g), to_float(b), to_float(a) };
			return value;
		}

		operator ImVec4() const
		{
			return { to_float(r), to_float(g), to_float(b), to_float(a) };
		}

		static color lerp(const color& start, const color& end, float t)
		{
			t = std::clamp(t, 0.0f, 1.0f);
			return color(
				static_cast<int>(start.r + (end.r - start.r) * t),
				static_cast<int>(start.g + (end.g - start.g) * t),
				static_cast<int>(start.b + (end.b - start.b) * t),
				static_cast<int>(start.a + (end.a - start.a) * t)
			);
		}
	private:
		static float to_float(uint8_t v)
		{
			return static_cast<float>(v & 0xFF) / 255.f;
		}

		static uint8_t from_float(float v)
		{
			return static_cast<uint8_t>(round(v * 255.f));
		}
	};

	enum class eJustify : uint8_t
	{
		Left,
		Right,
		Center
	};

	class timer
	{
	public:
		void start(const uint64_t time)
		{
			if (m_tick)
			{
				m_readyAt = GetTickCount64() + time;
				m_tick = false;
			}
		}

		bool ready() const
		{
			return GetTickCount64() > m_readyAt;
		}

		void reset()
		{
			m_tick = true;
		}

	private:
		uint64_t m_readyAt = 0;
		bool m_tick = false;
	};

	class sprite
	{
	public:
		std::string m_dictionary;
		std::string m_texture;
		float m_rotation;
	};

	class soundData
	{
	public:
		bool m_active;
		std::string_view m_id;
		std::string_view m_set = "HUD_FRONTEND_DEFAULT_SOUNDSET";
	};
}
