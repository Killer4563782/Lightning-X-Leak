#pragma once

constexpr long double pi = 3.141592653589793238462643383279502884L;
#define SIZEOF(a) (sizeof(a) / sizeof(std::remove_pointer_t<decltype(a)>))
#define COUNT(a) ((sizeof(a)/sizeof(0[a])) / ((u64)(!(sizeof(a) % sizeof(0[a])))))
#define ONCE_PER_FRAME(a) do a while (false)
#define ONCE(v, a) static bool v{ ([&] { a }(), true) };
#define HEX_TO_UPPER(value) "0x" << std::setfill('0') << std::setw(16) << std::hex << std::uppercase << (u64)value << std::nouppercase << std::dec << std::setw(0) << std::setfill(' ')
#define TRY_CATCH(a) try a catch(...){}