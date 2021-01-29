#pragma once

#include "memory.h"
#include <stdint.h>

namespace nes
{

enum class AddressMode
{
    Implicit,
    Accumulator,
    Immediate,
    ZeroPage,
    ZeroPageX,
    ZeroPageY,
    Relative,
    Absolute,
    AbsoluteX,
    AbsoluteY,
    Indirect,
    IndexedIndirect,
    IndirectIndexed,
};

struct Operand
{
    uint16_t value; // Don't need one of these. This should encap the address and address mode and a function will resolve the ACTUAL address
    uint16_t address;
    AddressMode addressMode;
};

class CPU;
typedef void (CPU::*Instruction)(Operand const&);

struct InstructionInfo
{
    Instruction instruction;
    nes::AddressMode addressMode;
    uint8_t instructionSize;
    uint8_t cycles;
    uint8_t pageCycles;
};

enum CpuFlags
{
    C = (1 << 0), // Carry
    Z = (1 << 1), // Zero
    I = (1 << 2), // Interrupt
    D = (1 << 3), // Decimal
    B = (1 << 4), // Break
    U = (1 << 5), // Unused
    V = (1 << 6), // Overflow
    N = (1 << 7), // Negative
};

struct CPU
{
	uint16_t pc;
	uint8_t a;
	uint8_t x;
	uint8_t y;
    uint8_t s;
    uint16_t sp; // Stack pointer
	Memory* const memoryBus;

	explicit CPU(Memory* const memory);
	~CPU() = default;
	// Rule of 5 here?

	void Step();

private:
	uint8_t Fetch(); // Read current opcode from PC, advance PC by instruction size
	Operand Decode(AddressMode addressMode) const;

	// Instructions from http://www.obelisk.me.uk/6502/instructions.html
    static InstructionInfo InstructionInfo[256];
    
	// Load/Store Operations
	void LDA(Operand const&);
	void LDX(Operand const&);
	void LDY(Operand const&);
	void STA(Operand const&);
	void STX(Operand const&);
	void STY(Operand const&);

	// Register Transfers
	void TAX(Operand const&);
	void TAY(Operand const&);
	void TXA(Operand const&);
	void TYA(Operand const&);

	// Stack Operations
	void TSX(Operand const&);
	void TXS(Operand const&);
	void PHA(Operand const&);
	void PHP(Operand const&);
	void PLA(Operand const&);
	void PLP(Operand const&);

	// Logical
	void AND(Operand const&);
	void EOR(Operand const&);
	void ORA(Operand const&);
	void BIT(Operand const&);

	// Arithmetic
	void ADC(Operand const&);
	void SBC(Operand const&);
	void CMP(Operand const&);
	void CPX(Operand const&);
	void CPY(Operand const&);

	// Increments & Decrements
	void INC(Operand const&);
	void INX(Operand const&);
	void INY(Operand const&);
	void DEC(Operand const&);
	void DEX(Operand const&);
	void DEY(Operand const&);

	// Shifts
	void ASL(Operand const&);
	void LSR(Operand const&);
	void ROL(Operand const&);
	void ROR(Operand const&);

	// Jumps & Calls
	void JMP(Operand const&);
	void JSR(Operand const&);
	void RTS(Operand const&);

	// Branches
	void BCC(Operand const&);
	void BCS(Operand const&);
	void BEQ(Operand const&);
	void BMI(Operand const&);
	void BNE(Operand const&);
	void BPL(Operand const&);
	void BVC(Operand const&);
	void BVS(Operand const&);

	// Status Flag Changes
	void CLC(Operand const&);
	void CLD(Operand const&);
	void CLV(Operand const&);
    void CLI(Operand const&);
	void SEC(Operand const&);
	void SED(Operand const&);
	void SEI(Operand const&);

	// System Functions
	void BRK(Operand const&);
	void NOP(Operand const&);
	void RTI(Operand const&);
};

} // nes
