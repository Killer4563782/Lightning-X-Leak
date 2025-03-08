#pragma once

namespace math
{
	template <typename T>
		requires std::_Is_any_of_v<T, float, double, long double>
	T random(T min, T max)
	{
		if (min == max)
			return min;

		if (min > max)
			std::swap(min, max);

		std::random_device rd;
		std::mt19937 gen(rd());

		std::uniform_real_distribution<T> distribution(min, max);

		return distribution(gen);
	}

	// Random number between 0 and max
	template <typename T>
		requires std::_Is_any_of_v<T, float, double, long double>
	T random(T max)
	{
		return random<T>(static_cast<T>(0), max);
	}


	// Formula: degrees * PI/180 = y rad
	inline float degrees_to_radians(const float degrees)
	{
		return static_cast<float>(static_cast<long double>(degrees) * pi / 180.L);
	}

	// Formula: radians * 180/PI = y deg
	inline float radians_to_degrees(const float radians)
	{
		return static_cast<float>(static_cast<long double>(radians) * 180.L / pi);
	}

	inline float dot_product(const Vector3& v1, const Vector3& v2)
	{
		return v1.x * v2.x + v1.y * v2.y;
	}

	inline float magnitude(const Vector3& v)
	{
		return sqrt(v.x * v.x + v.y * v.y);
	}

	inline float angle_between_vectors(const Vector3& from, const Vector3& to)
	{
		float dot = dot_product(from, to);
		float mag1 = magnitude(from);
		float mag2 = magnitude(to);

		float cos_theta = std::clamp(dot / (mag1 * mag2), -1.0f, 1.0f);
		return radians_to_degrees(acos(cos_theta));
	}

	inline Vector2 angle_to_rotation(const Vector3& v, double, float angle)
	{
		float angle_radians = degrees_to_radians(angle);
		float x = v.x * cos(angle_radians) - v.y * sin(angle_radians);
		float y = v.x * sin(angle_radians) + v.y * cos(angle_radians);
		return { x, y };
	}

	inline Vector3 rotation_to_direction(rage::fvector3 rot)
	{
		const float x_radian{ degrees_to_radians(rot.x) };
		const float z_radian{ degrees_to_radians(rot.z) };
		const float absolute_x_radian{ fabs(cos(x_radian)) };
		return { -sin(z_radian) * absolute_x_radian, cos(z_radian) * absolute_x_radian, sin(x_radian) };
	}

	inline Vector3 rotation_to_direction(const Vector3& rot)
	{
		const float x_radian{ degrees_to_radians(rot.x) };
		const float z_radian{ degrees_to_radians(rot.z) };
		const float absolute_x_radian{ fabs(cos(x_radian)) };
		return { -sin(z_radian) * absolute_x_radian, cos(z_radian) * absolute_x_radian, sin(x_radian) };
	}

	inline float distance_between_vectors(const Vector3& a, const Vector3& b)
	{
		return abs(a.x - b.x) + abs(a.y - b.y) + abs(a.z - b.z);
	}

	inline float distance_between_vectors(rage::fvector3 a, rage::fvector3 b)
	{
		return abs(a.x - b.x) + abs(a.y - b.y) + abs(a.z - b.z);
	}

	inline float distance_between_vectors(const rage::fvector3* a, const rage::fvector3* b)
	{
		return abs(a->x - b->x) + abs(a->y - b->y) + abs(a->z - b->z);
	}

	template <typename T>
	bool are_numbers_equal(T a, T b, T epsilon = std::numeric_limits<T>::epsilon())
	{
		return fabs(a - b) < epsilon;
	}

	inline bool are_floats_equal(const float a, const float b, const float epsilon = std::numeric_limits<float>::epsilon())
	{
		return fabs(a - b) < epsilon;
	}

	inline Vector3 quadratic_bezier_curve(Vector3& p0, Vector3& p1, Vector3& p2, float t)
	{
		const float one_minus_t{ 1.f - t };
		const float one_minus_t_squared{ one_minus_t * one_minus_t };
		const float t_squared{ t * t };
		return p0 * one_minus_t_squared + p1 * 2.f * one_minus_t * t + p2 * t_squared;
	}
}
