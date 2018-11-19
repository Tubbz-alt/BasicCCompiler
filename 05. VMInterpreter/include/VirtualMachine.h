#pragma once
#include <string>
#include <vector>

enum class OPCODE
{
	NOP = 0,
	FETCH,
	STORE,
	PUSH,
	POP,
	MUL,
	DIV,
	MOD,
	ADD,
	SUB,
	JMP_LT,
	JMP_LTEQ,
	JMP_GT,
	JMP_GTEQ,
	JMP_EQ,
	JMP_NEQ,
	LOGICALOR,
	LOGICALAND,
	_NOT,
	JMP,
	JZ,
	JNZ,
	PRTS,
	PRTC,
	PRTI,
	CALL,
	RET,
	SUB_REG,
	PUSHI,
	PUSHR,
	POPI,
	POPR,
	NEGATE,

	HLT,
};

#define MAX_BYTECODE_SIZE			4096
#define MAX_DATA_SIZE				1536
#define MAX_STACK_SIZE				256
#define MAX_RAM_SIZE				6144

#define CS_START_OFFSET				0
#define DS_START_OFFSET				CS_START_OFFSET + MAX_BYTECODE_SIZE
#define SS_START_OFFSET				DS_START_OFFSET + MAX_DATA_SIZE

#define READ_OPERAND(__eOpCode__)	readOperandFor(__eOpCode__)

enum EFLAGS_BIT
{
	CF = 0,		// Carry Flag. Set if the last arithmetic operation carried (addition) or borrowed (subtraction) a bit beyond the size of the register. 
				// This is then checked when the operation is followed with an add-with-carry or subtract-with-borrow to deal with values too large for just one register to contain.

	BIT_1,		// UNUSED, Don't Care.

	PF,			// Parity Flag. Set if the number of set bits in the least significant byte is a multiple of 2.

	BIT_3,		// UNUSED, Don't Care.

	AF,			// Adjust Flag. Carry of Binary Code Decimal (BCD) numbers arithmetic operations.

	BIT_5,		// UNUSED, Don't Care.

	ZF,			// Zero Flag. Set if the result of an operation is Zero (0).

	SF,			// Sign Flag. Set if the result of an operation is negative.

	TF,			// Trap Flag. Set if step by step debugging.

	IF,			// Interruption Flag. Set if interrupts are enabled.

	DF,			// Direction Flag. Stream direction. If set, string operations will decrement their pointer rather than incrementing it, reading memory backwards.

	OF,			// Overflow Flag. Set if signed arithmetic operations result in a value too large for the register to contain.

	IOPL_0,		// I/O Privilege Level field (2 bits). I/O Privilege Level of the current process.

	IOPL_1,		// I/O Privilege Level field (2 bits). I/O Privilege Level of the current process.

	NT,			// Nested Task flag. Controls chaining of interrupts. Set if the current process is linked to the next process.

	BIT_15,		// UNUSED, Don't Care.

	RF,			// Resume Flag. Response to debug exceptions.

	VM,			// Virtual-8086 Mode. Set if in 8086 compatibility mode.

	AC,			// Alignment Check. Set if alignment checking of memory references is done.

	VIF,		// Virtual Interrupt Flag. Virtual image of IF.

	VIP,		// Virtual Interrupt Pending flag. Set if an interrupt is pending.

	ID,			// Identification Flag. Support for CPUID instruction if can be set.
};

#define SET_FLAG(__EFlags__, __BIT__, __Value__)	(__EFlags__ |= (int)(1 << __BIT__));
#define IS_FLAG_SET(__EFlags__, __BIT__)			(__EFlags__ & (int)__BIT__ > 0)

enum class EREGISTERS
{
	RAX,	// Accumulator
	RCX,	// Counter
	RDX,	// Data
	RBX,	// Base
	RSP,	// Stack Pointer
	RBP,	// Stack Base Pointer
	RSI,	// Source
	RDI,	// Destination

	RMAX
};

typedef struct REGISTERS
{
	__int64		RAX;	// Accumulator
	__int64		RCX;	// Counter
	__int64		RDX;	// Data
	__int64		RBX;	// Base
	__int64		RSP;	// Stack Pointer
	__int64		RBP;	// Stack Base Pointer
	__int64		RSI;	// Source
	__int64		RDI;	// Destination

	__int32		CS;		// Code Segment Register
	__int32		DS;		// Data Segment Register
	__int32		SS;		// Stack Segment Register

	__int32		EFLAGS;
	__int32		EIP;
} REGISTERS;

class VirtualMachine
{
	public:
		static VirtualMachine*		create();

		static VirtualMachine*		m_pVMInstance;
		void						run(const char* sMachineCodeFile);
	protected:
		void						reset();
		int							loadBSS(const char* iByteCode, int startOffset, int iBuffLength);
		int							loadCode(const char* iByteCode, int startOffset, int iBuffLength);
		void						load(const char* iByteCode, int iBuffLength);
		void						execute(const char* iByteCode);
		OPCODE						fetch();
		void						eval(OPCODE eOpCode);
		int64_t						readOperandFor(OPCODE eOpCode);
	private:
									VirtualMachine();
		virtual						~VirtualMachine();

		REGISTERS					REGS;

		int8_t*						CODE;
		int32_t*					STACK;

		bool						m_bRunning;

		int8_t						RAM[MAX_RAM_SIZE];
};