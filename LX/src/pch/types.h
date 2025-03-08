#pragma once

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned long ul32;
typedef signed long sl32;
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long long u64;
typedef signed long long s64;
typedef float f32;
typedef double f64;
typedef long double f80;

template <typename Invokable, typename... invokableArgs>
concept is_invokable_with_args = requires(Invokable callable, const invokableArgs&... vaArgs)
{
	callable(vaArgs...);
};