#pragma once
#include <stdint.h>
#include "memory.h"

namespace nes
{
enum class AddressMode;
struct Operand;

struct CPU
{
	uint16_t PC;
	uint8_t A;
	uint8_t X;
	uint8_t Y;
	uint8_t Status;
	Memory* const MemoryBus;

	explicit CPU(Memory* const memory);
	~CPU() = default;
	// Rule of 5 here?

	void Step();

	void NOP(Operand const& operand); // This shouldn't be public...

private:
	uint8_t Fetch(); // Read current opcode from PC, advance PC by instruction size
	Operand Decode(AddressMode addressMode) const;

	// Instructions from http://www.obelisk.me.uk/6502/instructions.html

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
	void LSL(Operand const&);
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
	void SEC(Operand const&);
	void SED(Operand const&);
	void SEI(Operand const&);

	// System Functions
	void BRK(Operand const&);
	//void NOP(Operand const&);
	void RTI(Operand const&);
};

} // nes
