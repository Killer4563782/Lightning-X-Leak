#include "hooking/hooking.hpp"
#include "commands/manager.hpp"
#include "memory/pointers.h"
#include "services/script_patcher/script_patcher_service.hpp"
#include "util/globals.h"
#include <commands/settings.h>
#define CASE(c) case c:
#define DEFAULT default:
#define FETCH_INSN {
#define NEXT_INSN break; }
#define SET_PC(_o) ONCE_PER_FRAME({ s64 o{ _o }; pc = (opcodesTbl[o >> 14] + (o & 0x3FFF) - 1); opcodes = pc - o; })
#define ADD_PC(_r) SET_PC((pc - opcodes) + (_r))
#define CHECK_PC SET_PC(pc - opcodes)
#define Fault(s, ...) ONCE_PER_FRAME({ LOG(error, s, __VA_ARGS__); continue; })
#define LoadImm8 (*++pc)
#define LoadImm16 ((pc+=2), *(u16*)(pc-1))
#define LoadImmS16 ((pc+=2), *(s16*)(pc-1))
#define LoadImm24 ((pc+=3), *(u32*)(pc-3) >> 8)
#define LoadImm32 ((pc+=4), *(u32*)(pc-3))

static int old_cayo_flags;
static int old_shop_index;

class script_vm_guard
{
	rage::scrProgram* m_program;
	u8** m_orig_bytecode;

public:
	script_vm_guard(rage::scrProgram* program) : m_program(program)
	{
		m_orig_bytecode = program->m_code_blocks;

		if (auto bytecode = g_script_patcher_service->get_script_bytecode(program->m_name_hash))
			program->m_code_blocks = bytecode;

		if (g_pointers->g_globals[0xA])
		{
			scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[self::player].CurrentShopIndex = old_shop_index;
			scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[self::player].CayoPericoFlags = old_cayo_flags;
		}

#ifdef _DEBUG
		commands::settings::debug::in_script_vm = true;
#endif
	}

	~script_vm_guard()
	{
		m_program->m_code_blocks = m_orig_bytecode;

		if (g_pointers->g_globals[0xA])
		{
			old_shop_index = scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[self::player].CurrentShopIndex;
			old_cayo_flags = scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[self::player].CayoPericoFlags;

			if ("hide_from_player_list"_BC->m_toggle)
			{
				scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[self::player].CurrentShopIndex = -1;
				scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[self::player].CayoPericoFlags = 1;
			}
		}

#ifdef _DEBUG
		commands::settings::debug::in_script_vm = false;
#endif
	}
};
inline void scr_assign_string(char* dst, u32 size, const char* src)
{
	if (src)
	{
		while (*src && --size)
			*dst++ = *src++;
	}
	*dst = '\0';
}
inline void scr_append_string(char* dst, u32 size, const char* src)
{
	while (*dst)
		dst++, --size;
	scr_assign_string(dst, size, src);
}
inline void scr_itoa(char* dest, s32 value)
{
	char stack[16], *sp = stack;
	if (value < 0)
	{
		*dest++ = '-';
		value = -value;
	}
	else if (!value)
	{
		dest[0] = '0';
		dest[1] = 0;
		return;
	}
	while (value)
	{
		*sp++ = static_cast<char>(value % 10 + '0');
		value /= 10;
	}
	while (sp != stack)
		*dest++ = *--sp;
	*dest = 0;
}
inline f32 scr_fmodf(f32 x, f32 y)
{
	return y ? x - static_cast<s32>(x / y) * y : 0;
}
//	static u32 rage::scrThread::Run(rage::scrValue* __restrict stack, rage::scrValue* __restrict globals, const rage::scrProgram* pt, rage::scrThread::Serialized* __restrict ser);
rage::eThreadState hooks::script_vm(rage::scrValue* stack, rage::scrValue** globals, rage::scrProgram* pt, rage::scrThreadContext* ser)
{
	// Stop Background Script (bgscript, has funny little anti-cheat bs. stopping it from running does not cause any harm)
	if (ser->m_script_hash == "valentinerpreward2"_joaat)
	{
		return ser->m_state = rage::eThreadState::killed;
	}
	
	script_vm_guard guard(pt);
	return g_hooking->get_original<script_vm>()(stack, globals, pt, ser);

	//u8** opcodesTbl = pt->m_code_blocks;
	//rage::scrValue* sp = stack + ser->m_stack_pointer - 1;
	//rage::scrValue* fp = stack + ser->m_frame_pointer;
	//u8* pc{};
	//u8* opcodes{};
	//SET_PC(ser->m_pointer_count);
	////std::lock_guard lock(g_patches_mutex);
	////script_vm_guard guard(pt);
	//char buf[16]{};
	//while (true) {
	//	auto opcode = LoadImm8;
	//	switch (opcode) {
	//		CASE(OP_NOP) CHECK_PC; FETCH_INSN; NEXT_INSN;
	//		CASE(OP_IADD) FETCH_INSN; --sp; sp[0].Int += sp[1].Int; NEXT_INSN;
	//		CASE(OP_ISUB) FETCH_INSN; --sp; sp[0].Int -= sp[1].Int; NEXT_INSN;
	//		CASE(OP_IMUL) FETCH_INSN; --sp; sp[0].Int *= sp[1].Int; NEXT_INSN;
	//		CASE(OP_IDIV) FETCH_INSN; --sp; if (sp[1].Int) sp[0].Int /= sp[1].Int; NEXT_INSN;
	//		CASE(OP_IMOD) FETCH_INSN; --sp; if (sp[1].Int) sp[0].Int %= sp[1].Int; NEXT_INSN;
	//		CASE(OP_INOT) FETCH_INSN; sp[0].Int = !sp[0].Int; NEXT_INSN;
	//		CASE(OP_INEG) FETCH_INSN; sp[0].Int = -sp[0].Int; NEXT_INSN;

	//		CASE(OP_IEQ) FETCH_INSN; --sp; sp[0].Int = sp[0].Int == sp[1].Int; NEXT_INSN;
	//		CASE(OP_INE) FETCH_INSN; --sp; sp[0].Int = sp[0].Int != sp[1].Int; NEXT_INSN;
	//		CASE(OP_IGE) FETCH_INSN; --sp; sp[0].Int = sp[0].Int >= sp[1].Int; NEXT_INSN;
	//		CASE(OP_IGT) FETCH_INSN; --sp; sp[0].Int = sp[0].Int > sp[1].Int; NEXT_INSN;
	//		CASE(OP_ILE) FETCH_INSN; --sp; sp[0].Int = sp[0].Int <= sp[1].Int; NEXT_INSN;
	//		CASE(OP_ILT) FETCH_INSN; --sp; sp[0].Int = sp[0].Int < sp[1].Int; NEXT_INSN;

	//		CASE(OP_FADD) FETCH_INSN; --sp; sp[0].Float += sp[1].Float; NEXT_INSN;
	//		CASE(OP_FSUB) FETCH_INSN; --sp; sp[0].Float -= sp[1].Float; NEXT_INSN;
	//		CASE(OP_FMUL) FETCH_INSN; --sp; sp[0].Float *= sp[1].Float; NEXT_INSN;
	//		CASE(OP_FDIV) FETCH_INSN; --sp; if (sp[1].Int) sp[0].Float /= sp[1].Float; NEXT_INSN;
	//		CASE(OP_FMOD) FETCH_INSN; --sp; if (sp[1].Int) sp[0].Float = scr_fmodf(sp[0].Float, sp[1].Float); NEXT_INSN;
	//		CASE(OP_FNEG) FETCH_INSN; sp[0].Uns ^= static_cast<u32>(-1.f); NEXT_INSN;

	//		CASE(OP_FEQ) FETCH_INSN; --sp; sp[0].Int = sp[0].Float == sp[1].Float; NEXT_INSN;
	//		CASE(OP_FNE) FETCH_INSN; --sp; sp[0].Int = sp[0].Float != sp[1].Float; NEXT_INSN;
	//		CASE(OP_FGE) FETCH_INSN; --sp; sp[0].Int = sp[0].Float >= sp[1].Float; NEXT_INSN;
	//		CASE(OP_FGT) FETCH_INSN; --sp; sp[0].Int = sp[0].Float > sp[1].Float; NEXT_INSN;
	//		CASE(OP_FLE) FETCH_INSN; --sp; sp[0].Int = sp[0].Float <= sp[1].Float; NEXT_INSN;
	//		CASE(OP_FLT) FETCH_INSN; --sp; sp[0].Int = sp[0].Float < sp[1].Float; NEXT_INSN;

	//		CASE(OP_VADD) FETCH_INSN; sp -= 3; sp[-2].Float += sp[1].Float; sp[-1].Float += sp[2].Float; sp[0].Float += sp[3].Float; NEXT_INSN;
	//		CASE(OP_VSUB) FETCH_INSN; sp -= 3; sp[-2].Float -= sp[1].Float; sp[-1].Float -= sp[2].Float; sp[0].Float -= sp[3].Float; NEXT_INSN;
	//		CASE(OP_VMUL) FETCH_INSN; sp -= 3; sp[-2].Float *= sp[1].Float; sp[-1].Float *= sp[2].Float; sp[0].Float *= sp[3].Float; NEXT_INSN;
	//		CASE(OP_VDIV) FETCH_INSN; sp -= 3; if (sp[1].Int) sp[-2].Float /= sp[1].Float; if (sp[2].Int) sp[-1].Float /= sp[2].Float; if (sp[3].Int) sp[0].Float /= sp[3].Float; NEXT_INSN;
	//		CASE(OP_VNEG) FETCH_INSN; sp[-2].Uns ^= static_cast<u32>(-1.f); sp[-1].Uns ^= static_cast<u32>(-1.f); sp[0].Uns ^= static_cast<u32>(-1.f); NEXT_INSN;

	//		CASE(OP_IAND) FETCH_INSN; --sp; sp[0].Int &= sp[1].Int; NEXT_INSN;
	//		CASE(OP_IOR)  FETCH_INSN; --sp; sp[0].Int |= sp[1].Int; NEXT_INSN;
	//		CASE(OP_IXOR) FETCH_INSN; --sp; sp[0].Int ^= sp[1].Int; NEXT_INSN;

	//		CASE(OP_I2F) FETCH_INSN; sp[0].Float = static_cast<float>(sp[0].Int); NEXT_INSN;
	//		CASE(OP_F2I) FETCH_INSN; sp[0].Int = static_cast<s32>(sp[0].Float); NEXT_INSN;
	//		CASE(OP_F2V) FETCH_INSN; sp += 2; sp[-1].Int = sp[0].Int = sp[-2].Int; NEXT_INSN;

	//		CASE(OP_PUSH_CONST_U8) FETCH_INSN; ++sp; sp[0].Int = LoadImm8; NEXT_INSN;
	//		CASE(OP_PUSH_CONST_U8_U8) FETCH_INSN; sp += 2; sp[-1].Int = LoadImm8; sp[0].Int = LoadImm8; NEXT_INSN;
	//		CASE(OP_PUSH_CONST_U8_U8_U8) FETCH_INSN; sp += 3; sp[-2].Int = LoadImm8; sp[-1].Int = LoadImm8; sp[0].Int = LoadImm8; NEXT_INSN;

	//		CASE(OP_PUSH_CONST_U32) FETCH_INSN; ++sp; sp[0].Uns = LoadImm32; NEXT_INSN;
	//		CASE(OP_PUSH_CONST_F) FETCH_INSN; ++sp; sp[0].Uns = LoadImm32; NEXT_INSN;

	//		CASE(OP_DUP) FETCH_INSN; ++sp; sp[0].Any = sp[-1].Any; NEXT_INSN;
	//		CASE(OP_DROP) FETCH_INSN; --sp; NEXT_INSN;

	//		CASE(OP_NATIVE) FETCH_INSN;
	//			int returnSize = LoadImm8;
	//			int paramCount = (returnSize >> 2) & 63;
	//			u32 imm = (LoadImm8 << 8);
	//			imm |= LoadImm8;
	//			returnSize &= 3;
	//			rage::scrNativeHandler cmd = (rage::scrNativeHandler)(size_t)pt->m_native_entrypoints[imm];
	//			ser->m_pointer_count = (s32)(pc - opcodes - 4);
	//			ser->m_frame_pointer = (s32)(fp - stack);
	//			ser->m_stack_pointer = (s32)(sp - stack + 1);
	//			rage::scrNativeCallContext curInfo(returnSize ? &stack[ser->m_stack_pointer - paramCount] : 0, paramCount, &stack[ser->m_stack_pointer - paramCount]);
	//			cmd(&curInfo);
	//			if (ser->m_state != rage::eThreadState::running)
	//				return ser->m_state;
	//			curInfo.copy_params_out();
	//			sp -= paramCount - returnSize;
	//		NEXT_INSN;

	//		CASE(OP_ENTER) FETCH_INSN;
	//			u32 paramCount = LoadImm8;
	//			u32 localCount = LoadImm16;
	//			u32 nameCount = LoadImm8;
	//			if (ser->m_call_depth < 16) {
	//				ser->m_callstack[ser->m_call_depth] = (s32)(pc - opcodes + 2);
	//			}
	//			++(ser->m_call_depth);
	//			pc += nameCount;
	//			if (sp - stack >= (s32)(ser->m_stack_size - localCount))
	//				Fault("Stack overflow");
	//			(++sp)->Int = (s32)(fp - stack);
	//			fp = sp - paramCount - 1;
	//			while (localCount--)
	//				(++sp)->Any = 0;
	//			sp -= paramCount;
	//		NEXT_INSN;

	//		CASE(OP_LEAVE) FETCH_INSN;
	//			--(ser->m_call_depth);
	//			u32 paramCount = LoadImm8; // Get number of parameters to clean off the stack
	//			u32 returnSize = LoadImm8; // Get size of the return value
	//			rage::scrValue *result = sp - returnSize; // Remember start of return value
	//			sp = fp + paramCount - 1;
	//			fp = stack + sp[2].Uns;
	//			u32 newPc{ sp[1].Uns };
	//			SET_PC(newPc);
	//			sp -= paramCount;
	//			while (returnSize--) {
	//				*++sp = *++result;
	//			}
	//			if (!newPc) {
	//				return ser->m_state = rage::eThreadState::aborted;
	//			}
	//		NEXT_INSN;

	//		CASE(OP_LOAD) FETCH_INSN; sp[0].Any = (sp[0].Reference)->Any; NEXT_INSN;
	//		CASE(OP_STORE) FETCH_INSN; sp -= 2; (sp[2].Reference)->Any = sp[1].Any; NEXT_INSN;
	//		CASE(OP_STORE_REV) FETCH_INSN; --sp; (sp[0].Reference)->Any = sp[1].Any; NEXT_INSN;

	//		CASE(OP_LOAD_N) FETCH_INSN;
	//			rage::scrValue *addr = ((sp--)->Reference);
	//			u32 count = (sp--)->Int;
	//			for (u32 i=0; i<count; i++)
	//				(++sp)->Any = addr[i].Any;
	//		NEXT_INSN;

	//		CASE(OP_STORE_N) FETCH_INSN;
	//			// Values are pushed up to the stack in evaluation order,
	//			// so reverse them when storing
	//			rage::scrValue* addr = ((sp--)->Reference);
	//			u32 count = (sp--)->Int;
	//			for (u32 i=0; i<count; i++)
	//				addr[count-1-i].Any = (sp--)->Any;
	//		NEXT_INSN;

	//		CASE(OP_ARRAY_U8) FETCH_INSN;
	//			--sp;
	//			rage::scrValue* r{ sp[1].Reference };
	//			size_t idx{ sp[0].Uns };
	//			if (idx >= r->Uns) {
	//				Fault("Array overrun, {} >= {}", idx, r->Uns);
	//			}
	//			r += 1U + idx * LoadImm8;
	//			sp[0].Reference = r;
	//		NEXT_INSN;

	//		CASE(OP_ARRAY_U8_LOAD) FETCH_INSN;
	//			--sp;
	//			rage::scrValue* r{ sp[1].Reference };
	//			size_t idx{ sp[0].Uns };
	//			if (idx >= r->Uns) {
	//				Fault("Array overrun, {} >= {}", idx, r->Uns);
	//			}
	//			r += 1U + idx * LoadImm8;
	//			sp[0].Any = r->Any;
	//		NEXT_INSN;

	//		CASE(OP_ARRAY_U8_STORE) FETCH_INSN;
	//			sp -= 3;
	//			rage::scrValue* r{ sp[3].Reference };
	//			size_t idx{ sp[2].Uns };
	//			if (idx >= r->Uns) {
	//				Fault("Array overrun, {} >= {}", idx, r->Uns);
	//			}
	//			r += 1U + idx * LoadImm8;
	//			r->Any = sp[1].Any;
	//		NEXT_INSN;

	//		CASE(OP_LOCAL_U8) FETCH_INSN; ++sp; sp[0].Reference = (fp + LoadImm8); NEXT_INSN;
	//		CASE(OP_LOCAL_U8_LOAD) FETCH_INSN; ++sp; sp[0].Any = fp[LoadImm8].Any; NEXT_INSN;
	//		CASE(OP_LOCAL_U8_STORE) FETCH_INSN; --sp; fp[LoadImm8].Any = sp[1].Any; NEXT_INSN;

	//		CASE(OP_STATIC_U8) FETCH_INSN; ++sp; sp[0].Reference = (stack + LoadImm8); NEXT_INSN;
	//		CASE(OP_STATIC_U8_LOAD) FETCH_INSN; ++sp; sp[0].Any = stack[LoadImm8].Any; NEXT_INSN;
	//		CASE(OP_STATIC_U8_STORE) FETCH_INSN; --sp; stack[LoadImm8].Any = sp[1].Any; NEXT_INSN;

	//		CASE(OP_IADD_U8) FETCH_INSN; sp[0].Int += LoadImm8; NEXT_INSN;
	//		CASE(OP_IMUL_U8) FETCH_INSN; sp[0].Int *= LoadImm8; NEXT_INSN;

	//		CASE(OP_IOFFSET) FETCH_INSN; --sp; sp[0].Any += sp[1].Int * sizeof(rage::scrValue); NEXT_INSN;
	//		CASE(OP_IOFFSET_U8) FETCH_INSN; sp[0].Any += LoadImm8 * sizeof(rage::scrValue); NEXT_INSN;
	//		CASE(OP_IOFFSET_U8_LOAD) FETCH_INSN; sp[0].Any = (sp[0].Reference)[LoadImm8].Any; NEXT_INSN;
	//		CASE(OP_IOFFSET_U8_STORE) FETCH_INSN; sp -= 2; (sp[2].Reference)[LoadImm8].Any = sp[1].Any; NEXT_INSN;

	//		CASE(OP_PUSH_CONST_S16) FETCH_INSN; ++sp; sp[0].Int = LoadImmS16; NEXT_INSN;
	//		CASE(OP_IADD_S16) FETCH_INSN; sp[0].Int += LoadImmS16; NEXT_INSN;
	//		CASE(OP_IMUL_S16) FETCH_INSN; sp[0].Int *= LoadImmS16; NEXT_INSN;

	//		CASE(OP_IOFFSET_S16) FETCH_INSN; sp[0].Any += LoadImmS16 * sizeof(rage::scrValue); NEXT_INSN;
	//		CASE(OP_IOFFSET_S16_LOAD) FETCH_INSN; sp[0].Any = (sp[0].Reference)[LoadImmS16].Any; NEXT_INSN;
	//		CASE(OP_IOFFSET_S16_STORE) FETCH_INSN; sp -= 2; (sp[2].Reference)[LoadImmS16].Any = sp[1].Any; NEXT_INSN;

	//		CASE(OP_ARRAY_U16) FETCH_INSN;
	//			--sp;
	//			rage::scrValue* r{ sp[1].Reference };
	//			size_t idx{ sp[0].Uns };
	//			if (idx >= r->Uns) {
	//				Fault("Array overrun, {} >= {}", idx, r->Uns);
	//			}
	//			r += 1U + idx * LoadImm16;
	//			sp[0].Reference = r;
	//		NEXT_INSN;

	//		CASE(OP_ARRAY_U16_LOAD) FETCH_INSN;
	//			--sp;
	//			rage::scrValue* r{ sp[1].Reference };
	//			size_t idx{ sp[0].Uns };
	//			if (idx >= r->Uns) {
	//				Fault("Array overrun, {} >= {}", idx, r->Uns);
	//			}
	//			r += 1U + idx * LoadImm16;
	//			sp[0].Any = r->Any;
	//		NEXT_INSN;

	//		CASE(OP_ARRAY_U16_STORE) FETCH_INSN;
	//			sp -= 3;
	//			rage::scrValue* r{ sp[3].Reference };
	//			u32 idx{ sp[2].Uns };
	//			if (idx >= r->Uns) {
	//				Fault("Array overrun, {} >= {}", idx, r->Uns);
	//			}
	//			r += 1U + idx * LoadImm16;
	//			r->Any = sp[1].Any;
	//		NEXT_INSN;

	//		CASE(OP_LOCAL_U16) FETCH_INSN; ++sp; sp[0].Reference = (fp + LoadImm16); NEXT_INSN;
	//		CASE(OP_LOCAL_U16_LOAD) FETCH_INSN; ++sp; sp[0].Any = fp[LoadImm16].Any; NEXT_INSN;
	//		CASE(OP_LOCAL_U16_STORE) FETCH_INSN; --sp; fp[LoadImm16].Any = sp[1].Any; NEXT_INSN;

	//		CASE(OP_STATIC_U16) FETCH_INSN; ++sp; sp[0].Reference = (stack + LoadImm16); NEXT_INSN;
	//		CASE(OP_STATIC_U16_LOAD) FETCH_INSN; ++sp; sp[0].Any = stack[LoadImm16].Any; NEXT_INSN;
	//		CASE(OP_STATIC_U16_STORE) FETCH_INSN; --sp; stack[LoadImm16].Any = sp[1].Any; NEXT_INSN;

	//		CASE(OP_GLOBAL_U16) FETCH_INSN; ++sp; sp[0].Reference = (globals[0] + LoadImm16); NEXT_INSN;
	//		CASE(OP_GLOBAL_U16_LOAD) FETCH_INSN; ++sp; sp[0].Any = globals[0][LoadImm16].Any; NEXT_INSN;
	//		CASE(OP_GLOBAL_U16_STORE) FETCH_INSN; --sp; globals[0][LoadImm16].Any = sp[1].Any; NEXT_INSN;

	//		CASE(OP_STATIC_U24) FETCH_INSN; ++sp; sp[0].Reference = (stack + LoadImm24); NEXT_INSN;
	//		CASE(OP_STATIC_U24_LOAD) FETCH_INSN; ++sp; sp[0].Any = stack[LoadImm24].Any; NEXT_INSN;
	//		CASE(OP_STATIC_U24_STORE) FETCH_INSN; --sp; stack[LoadImm24].Any = sp[1].Any; NEXT_INSN;

	//		CASE(OP_GLOBAL_U24) FETCH_INSN;
	//			u32 imm{ LoadImm24 }, block{ imm >> 18 }, ofs{ imm & 0x3FFFF };
	//			++sp;
	//			if (!globals[block]) {
	//				Fault("Global block {}/{} not resident", block, ofs);
	//				return ser->m_state = rage::eThreadState::halted;
	//			}
	//			else {
	//				sp[0].Reference = (&globals[block][ofs]);
	//			}
	//		NEXT_INSN;

	//		CASE(OP_GLOBAL_U24_LOAD) FETCH_INSN;
	//			u32 imm = LoadImm24, block = imm >> rage::scrProgram::MAX_GLOBAL_BLOCKS_SHIFT, ofs = imm & rage::scrProgram::GLOBAL_SIZE_MASK; 
	//			++sp; 
	//			if (!globals[block]) 
	//				Fault("Global block %u/%u not resident",block,ofs); 
	//			else
	//				sp[0].Any = globals[block][ofs].Any;
	//		NEXT_INSN;

	//		CASE(OP_GLOBAL_U24_STORE) FETCH_INSN;
	//			u32 imm = LoadImm24, block = imm >> rage::scrProgram::MAX_GLOBAL_BLOCKS_SHIFT, ofs = imm & rage::scrProgram::GLOBAL_SIZE_MASK; 
	//			--sp;
	//			if (!globals[block]) 
	//				Fault("Global block %u/%u not resident",block,ofs); 
	//			else
	//				globals[block][ofs].Any = sp[1].Any; 
	//		NEXT_INSN;
	//		CASE(OP_PUSH_CONST_U24) FETCH_INSN; ++sp; sp[0].Int = LoadImm24; NEXT_INSN;

	//		CASE(OP_CALL) FETCH_INSN; u32 imm = LoadImm24; ++sp; sp[0].Uns = (s32)(pc - opcodes); SET_PC(imm); NEXT_INSN;

	//		CASE(OP_J) FETCH_INSN; s32 imm{ LoadImmS16 }; ADD_PC(imm); NEXT_INSN;
	//		CASE(OP_JZ) FETCH_INSN; s32 imm{ LoadImmS16 }; --sp; if (sp[1].Int == 0) ADD_PC(imm); else CHECK_PC; NEXT_INSN;
	//		CASE(OP_IEQ_JZ) FETCH_INSN; s32 imm{ LoadImmS16 }; sp -= 2; if (!(sp[1].Int == sp[2].Int)) ADD_PC(imm); else CHECK_PC; NEXT_INSN;
	//		CASE(OP_INE_JZ) FETCH_INSN; s32 imm{ LoadImmS16 }; sp -= 2; if (!(sp[1].Int != sp[2].Int)) ADD_PC(imm); else CHECK_PC; NEXT_INSN;
	//		CASE(OP_IGE_JZ) FETCH_INSN; s32 imm{ LoadImmS16 }; sp -= 2; if (!(sp[1].Int >= sp[2].Int)) ADD_PC(imm); else CHECK_PC; NEXT_INSN;
	//		CASE(OP_IGT_JZ) FETCH_INSN; s32 imm{ LoadImmS16 }; sp -= 2; if (!(sp[1].Int > sp[2].Int)) ADD_PC(imm); else CHECK_PC; NEXT_INSN;
	//		CASE(OP_ILE_JZ) FETCH_INSN; s32 imm{ LoadImmS16 }; sp -= 2; if (!(sp[1].Int <= sp[2].Int)) ADD_PC(imm); else CHECK_PC; NEXT_INSN;
	//		CASE(OP_ILT_JZ) FETCH_INSN; s32 imm{ LoadImmS16 }; sp -= 2; if (!(sp[1].Int < sp[2].Int)) ADD_PC(imm); else CHECK_PC; NEXT_INSN;

	//		CASE(OP_SWITCH) FETCH_INSN;
	//			--sp;
	//			u32 label{ sp[1].Uns };
	//			u32 count{ LoadImm8 };
	//			CHECK_PC;
	//			for (u32 i{}; i != count; ++i) {
	//				u32 match{ LoadImm32 };
	//				u32 target{ LoadImm16 };
	//				if (label == match) {
	//					ADD_PC(target);
	//					break;
	//				}
	//			}
	//			CHECK_PC;
	//		NEXT_INSN;

	//		CASE(OP_STRING) FETCH_INSN;
	//			u32 offset{ sp[0].Uns };
	//			sp[0].String = (pt->m_strings_data[offset >> 14] + (offset & 0x3FFF));
	//		NEXT_INSN;

	//		CASE(OP_TEXT_LABEL_ASSIGN_STRING) FETCH_INSN;
	//			sp -= 2;
	//			char* dest{ const_cast<char*>(sp[2].String) };
	//			const char* src{ sp[1].String };
	//			scr_assign_string(dest, LoadImm8, src);
	//		NEXT_INSN;

	//		CASE(OP_TEXT_LABEL_ASSIGN_INT) FETCH_INSN;
	//			sp -= 2;
	//			char* dest{ const_cast<char*>(sp[2].String) };
	//			s32 value{ sp[1].Int };
	//			scr_itoa(buf, value);
	//			scr_assign_string(dest, LoadImm8, buf);
	//		NEXT_INSN;

	//		CASE(OP_TEXT_LABEL_APPEND_STRING) FETCH_INSN;
	//			sp -= 2;
	//			char* dest{ const_cast<char*>(sp[2].String) };
	//			const char* src{ sp[1].String };
	//			scr_append_string(dest, LoadImm8, src);
	//		NEXT_INSN;

	//		CASE(OP_TEXT_LABEL_APPEND_INT) FETCH_INSN;
	//			sp -= 2;
	//			char* dest{ const_cast<char*>(sp[2].String) };
	//			s32 value{ sp[1].Int };
	//			scr_itoa(buf, value);
	//			scr_append_string(dest, LoadImm8, buf);
	//		NEXT_INSN;

	//		CASE(OP_TEXT_LABEL_COPY) FETCH_INSN;
	//			sp -= 3;
	//			rage::scrValue* dest{ sp[3].Reference };
	//			s32 destSize{ sp[2].Int };
	//			s32 srcSize{ sp[1].Int };
	//			//Remove excess
	//			while (srcSize > destSize) {
	//				--srcSize;
	//				--sp;
	//			}
	//			//Do the bulk of the copy
	//			for (s32 i{}; i != srcSize; ++i)
	//				dest[srcSize - 1 - i].Reference = (sp--)->Reference;
	//			//Make sure it's still NUL-terminated
	//			char* cDest{ (char*)dest };
	//			cDest[(srcSize * sizeof(rage::scrValue)) - 1] = '\0';
	//		NEXT_INSN;

	//		CASE(OP_CATCH) FETCH_INSN;
	//			ser->m_catch_pointer_count = (s32)(pc - opcodes);
	//			ser->m_catch_frame_pointer = (s32)(fp - stack);
	//			ser->m_catch_stack_pointer = (s32)(sp - stack + 1);
	//			++sp;
	//			sp[0].Int = -1;
	//		NEXT_INSN;

	//		CASE(OP_THROW) FETCH_INSN;
	//			s32 imm{ sp[0].Int };
	//			if (!ser->m_catch_pointer_count) {
	//				Fault("THROW with no CATCH");
	//			}
	//			else {
	//				SET_PC(ser->m_catch_pointer_count);
	//				fp = stack + ser->m_catch_frame_pointer;
	//				sp = stack + ser->m_catch_stack_pointer;
	//				sp[0].Int = imm;
	//			}
	//		NEXT_INSN;

	//		CASE(OP_CALLINDIRECT) FETCH_INSN;
	//			u32 imm{ sp[0].Uns };
	//			if (!imm)
	//				Fault("Attempted to call through uninitialized (zero) function pointer");
	//			sp[0].Uns = (s32)(pc - opcodes);
	//			SET_PC(imm);
	//		NEXT_INSN;

	//		CASE(OP_PUSH_CONST_M1) FETCH_INSN; ++sp; sp[0].Int = -1; NEXT_INSN;
	//		CASE(OP_PUSH_CONST_0) FETCH_INSN; ++sp; sp[0].Any = 0; NEXT_INSN;
	//		CASE(OP_PUSH_CONST_1) FETCH_INSN; ++sp; sp[0].Int = 1; NEXT_INSN;
	//		CASE(OP_PUSH_CONST_2) FETCH_INSN; ++sp; sp[0].Int = 2; NEXT_INSN;
	//		CASE(OP_PUSH_CONST_3) FETCH_INSN; ++sp; sp[0].Int = 3; NEXT_INSN;
	//		CASE(OP_PUSH_CONST_4) FETCH_INSN; ++sp; sp[0].Int = 4; NEXT_INSN;
	//		CASE(OP_PUSH_CONST_5) FETCH_INSN; ++sp; sp[0].Int = 5; NEXT_INSN;
	//		CASE(OP_PUSH_CONST_6) FETCH_INSN; ++sp; sp[0].Int = 6; NEXT_INSN;
	//		CASE(OP_PUSH_CONST_7) FETCH_INSN; ++sp; sp[0].Int = 7; NEXT_INSN;

	//		CASE(OP_PUSH_CONST_FM1) FETCH_INSN; ++sp; sp[0].Float = -1.f; NEXT_INSN;
	//		CASE(OP_PUSH_CONST_F0) FETCH_INSN; ++sp; sp[0].Float = 0.f; NEXT_INSN;
	//		CASE(OP_PUSH_CONST_F1) FETCH_INSN; ++sp; sp[0].Float = 1.f; NEXT_INSN;
	//		CASE(OP_PUSH_CONST_F2) FETCH_INSN; ++sp; sp[0].Float = 2.f; NEXT_INSN;
	//		CASE(OP_PUSH_CONST_F3) FETCH_INSN; ++sp; sp[0].Float = 3.f; NEXT_INSN;
	//		CASE(OP_PUSH_CONST_F4) FETCH_INSN; ++sp; sp[0].Float = 4.f; NEXT_INSN;
	//		CASE(OP_PUSH_CONST_F5) FETCH_INSN; ++sp; sp[0].Float = 5.f; NEXT_INSN;
	//		CASE(OP_PUSH_CONST_F6) FETCH_INSN; ++sp; sp[0].Float = 6.f; NEXT_INSN;
	//		CASE(OP_PUSH_CONST_F7) FETCH_INSN; ++sp; sp[0].Float = 7.f; NEXT_INSN;

	//		CASE(OP_STRINGHASH) FETCH_INSN; sp[0].Uns = rage::joaat(sp[0].String); NEXT_INSN;

	//		CASE(OP_IS_BIT_SET) FETCH_INSN;
	//			--sp;
	//			sp[0].Int = ((sp[0].Int & (1 << sp[1].Int)) != 0);
	//		NEXT_INSN;

	//		DEFAULT Fault("Invalid opcode {} (0x{:X})", opcode, opcode);
	//	}
	//}
}
