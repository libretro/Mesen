#include "stdafx.h"
#include "CPU.h"
#include "PPU.h"
#include "APU.h"

CPU* CPU::Instance = nullptr;

CPU::CPU(MemoryManager *memoryManager) : _memoryManager(memoryManager)
{
	CPU::Instance = this;

	Func opTable[] = { 
	//	0				1				2				3				4				5				6						7				8				9				A						B				C						D						E						F
		&CPU::BRK,	&CPU::ORA,	nullptr,		nullptr,		&CPU::NOP,	&CPU::ORA,	&CPU::ASL_Memory,	nullptr,		&CPU::PHP,	&CPU::ORA,	&CPU::ASL_Acc,		nullptr,		&CPU::NOP,			&CPU::ORA,	&CPU::ASL_Memory,		nullptr, //0
		&CPU::BPL,	&CPU::ORA,	nullptr,		nullptr,		&CPU::NOP,	&CPU::ORA,	&CPU::ASL_Memory,	nullptr,		&CPU::CLC,	&CPU::ORA,	nullptr,				nullptr,		&CPU::NOP,			&CPU::ORA,	&CPU::ASL_Memory,		nullptr, //1
		&CPU::JSR,	&CPU::AND,	nullptr,		nullptr,		&CPU::BIT,	&CPU::AND,	&CPU::ROL_Memory,	nullptr,		&CPU::PLP,	&CPU::AND,	&CPU::ROL_Acc,		nullptr,		&CPU::BIT,			&CPU::AND,	&CPU::ROL_Memory,		nullptr, //2
		&CPU::BMI,	&CPU::AND,	nullptr,		nullptr,		&CPU::NOP,	&CPU::AND,	&CPU::ROL_Memory,	nullptr,		&CPU::SEC,	&CPU::AND,	nullptr,				nullptr,		&CPU::NOP,			&CPU::AND,	&CPU::ROL_Memory,		nullptr, //3
		&CPU::RTI,	&CPU::EOR,	nullptr,		nullptr,		&CPU::NOP,	&CPU::EOR,	&CPU::LSR_Memory,	nullptr,		&CPU::PHA,	&CPU::EOR,	&CPU::LSR_Acc,		nullptr,		&CPU::JMP_Abs,		&CPU::EOR,	&CPU::LSR_Memory,		nullptr, //4
		&CPU::BVC,	&CPU::EOR,	nullptr,		nullptr,		&CPU::NOP,	&CPU::EOR,	&CPU::LSR_Memory,	nullptr,		&CPU::CLI,	&CPU::EOR,	nullptr,				nullptr,		&CPU::NOP,			&CPU::EOR,	&CPU::LSR_Memory,		nullptr, //5
		&CPU::RTS,	&CPU::ADC,	nullptr,		nullptr,		&CPU::NOP,	&CPU::ADC,	&CPU::ROR_Memory,	nullptr,		&CPU::PLA,	&CPU::ADC,	&CPU::ROR_Acc,		nullptr,		&CPU::JMP_Ind,		&CPU::ADC,	&CPU::ROR_Memory,		nullptr, //6
		&CPU::BVS,	&CPU::ADC,	nullptr,		nullptr,		&CPU::NOP,	&CPU::ADC,	&CPU::ROR_Memory,	nullptr,		&CPU::SEI,	&CPU::ADC,	nullptr,				nullptr,		&CPU::NOP,			&CPU::ADC,	&CPU::ROR_Memory,		nullptr, //7
		&CPU::NOP,	&CPU::STA,	&CPU::NOP,	nullptr,		&CPU::STY,	&CPU::STA,	&CPU::STX,			nullptr,		&CPU::DEY,	&CPU::NOP,	&CPU::TXA,			nullptr,		&CPU::STY,			&CPU::STA,	&CPU::STX,				nullptr, //8
		&CPU::BCC,	&CPU::STA,	nullptr,		nullptr,		&CPU::STY,	&CPU::STA,	&CPU::STX,			nullptr,		&CPU::TYA,	&CPU::STA,	&CPU::TXS,			nullptr,		nullptr,				&CPU::STA,	nullptr,					nullptr, //9
		&CPU::LDY,	&CPU::LDA,	&CPU::LDX,	nullptr,		&CPU::LDY,	&CPU::LDA,	&CPU::LDX,			nullptr,		&CPU::TAY,	&CPU::LDA,	&CPU::TAX,			nullptr,		&CPU::LDY,			&CPU::LDA,	&CPU::LDX,				nullptr, //A
		&CPU::BCS,	&CPU::LDA,	nullptr,		nullptr,		&CPU::LDY,	&CPU::LDA,	&CPU::LDX,			nullptr,		&CPU::CLV,	&CPU::LDA,	&CPU::TSX,			nullptr,		&CPU::LDY,			&CPU::LDA,	&CPU::LDX,				nullptr, //B
		&CPU::CPY,	&CPU::CPA,	&CPU::NOP,	nullptr,		&CPU::CPY,	&CPU::CPA,	&CPU::DEC,			nullptr,		&CPU::INY,	&CPU::CPA,	&CPU::DEX,			nullptr,		&CPU::CPY,			&CPU::CPA,	&CPU::DEC,				nullptr, //C
		&CPU::BNE,	&CPU::CPA,	nullptr,		nullptr,		&CPU::NOP,	&CPU::CPA,	&CPU::DEC,			nullptr,		&CPU::CLD,	&CPU::CPA,	nullptr,				nullptr,		&CPU::NOP,			&CPU::CPA,	&CPU::DEC,				nullptr, //D
		&CPU::CPX,	&CPU::SBC,	&CPU::NOP,	nullptr,		&CPU::CPX,	&CPU::SBC,	&CPU::INC,			nullptr,		&CPU::INX,	&CPU::SBC,	&CPU::NOP,			nullptr,		&CPU::CPX,			&CPU::SBC,	&CPU::INC,				nullptr, //E
		&CPU::BEQ,	&CPU::SBC,	nullptr,		nullptr,		&CPU::NOP,	&CPU::SBC,	&CPU::INC,			nullptr,		&CPU::SED,	&CPU::SBC,	nullptr,				nullptr,		&CPU::NOP,			&CPU::SBC,	&CPU::INC,				nullptr  //F
	};

	AddrMode addrMode[] = {
	//	0		1		 2		 3		  4		5		 6		  7		8	  9		A	  B		C		 D		  E		F
		Imp,  IndX,  None, IndX,  Zero,  Zero,  Zero,  Zero,  Imp, Imm,   Imp, Imm,   Abs,   Abs,   Abs,   Abs,   //0
		Rel,  IndY,  None, IndYW, ZeroX, ZeroX, ZeroX, ZeroX, Imp, AbsY,  Imp, AbsYW, AbsX,  AbsX,  AbsXW, AbsXW, //1
		Abs, IndX,  None, IndX,  Zero,  Zero,  Zero,  Zero,  Imp, Imm,   Imp, Imm,   Abs,   Abs,   Abs,   Abs,    //2
		Rel,  IndY,  None, IndYW, ZeroX, ZeroX, ZeroX, ZeroX, Imp, AbsY,  Imp, AbsYW, AbsX,  AbsX,  AbsXW, AbsXW, //3
		Imp,  IndX,  None, IndX,  Zero,  Zero,  Zero,  Zero,  Imp, Imm,   Imp, Imm,   Abs,   Abs,   Abs,   Abs,   //4
		Rel,  IndY,  None, IndYW, ZeroX, ZeroX, ZeroX, ZeroX, Imp, AbsY,  Imp, AbsYW, AbsX,  AbsX,  AbsXW, AbsXW, //5
		Imp,  IndX,  None, IndX,  Zero,  Zero,  Zero,  Zero,  Imp, Imm,   Imp, Imm,   Abs,   Abs,   Abs,   Abs,   //6
		Rel,  IndY,  None, IndYW, ZeroX, ZeroX, ZeroX, ZeroX, Imp, AbsY,  Imp, AbsYW, AbsX,  AbsX,  AbsXW, AbsXW, //7
		Imm,  IndX,  Imm,  IndX,  Zero,  Zero,  Zero,  Zero,  Imp, Imm,   Imp, Imm,   Abs,   Abs,   Abs,   Abs,   //8
		Rel,  IndYW, None, IndY,  ZeroX, ZeroX, ZeroY, ZeroY, Imp, AbsYW, Imp, AbsY,  AbsXW, AbsXW, AbsYW, AbsY,  //9
		Imm,  IndX,  Imm,  IndX,  Zero,  Zero,  Zero,  Zero,  Imp, Imm,   Imp, Imm,   Abs,   Abs,   Abs,   Abs,   //A
		Rel,  IndY,  None, IndY,  ZeroX, ZeroX, ZeroY, ZeroY, Imp, AbsY,  Imp, AbsY,  AbsX,  AbsX,  AbsY,  AbsY,  //B
		Imm,  IndX,  Imm,  IndX,  Zero,  Zero,  Zero,  Zero,  Imp, Imm,   Imp, Imm,   Abs,   Abs,   Abs,   Abs,   //C
		Rel,  IndY,  None, IndYW, ZeroX, ZeroX, ZeroX, ZeroX, Imp, AbsY,  Imp, AbsYW, AbsX,  AbsX,  AbsXW, AbsXW, //D
		Imm,  IndX,  Imm,  IndX,  Zero,  Zero,  Zero,  Zero,  Imp, Imm,   Imp, Imm,   Abs,   Abs,   Abs,   Abs,   //E
		Rel,  IndY,  None, IndYW, ZeroX, ZeroX, ZeroX, ZeroX, Imp, AbsY,  Imp, AbsYW, AbsX,  AbsX,  AbsXW, AbsXW, //F
	};
	
	memcpy(_opTable, opTable, sizeof(opTable));
	memcpy(_addrMode, addrMode, sizeof(addrMode));
}

void CPU::Reset(bool softReset)
{
	_state.NMIFlag = false;
	_state.IRQFlag = 0;
	_cycleCount = 0;
	_relativeCycleCount = 0;

	_state.PC = MemoryReadWord(CPU::ResetVector);
	if(softReset) {
		SetFlags(PSFlags::Interrupt);
		_state.SP -= 0x03;
	} else {
		_state.A = 0;
		_state.SP = 0xFD;
		_state.X = 0;
		_state.Y = 0;
		_state.PS = PSFlags::Reserved | PSFlags::Interrupt;

		_runIRQ = false;
		_runNMI = false;
	}
}

uint32_t CPU::Exec()
{
	if(!_runNMI && !_runIRQ) {
		uint8_t opCode = GetOPCode();
		if(_state.NMIFlag) {
			_runNMI = true;
		} else if(opCode != 0x40 && _state.IRQFlag > 0 && !CheckFlag(PSFlags::Interrupt)) {
			_runIRQ = true;
		}

		_instAddrMode = _addrMode[opCode];
		_operand = FetchOperand();

		if(_opTable[opCode] != nullptr) {
			(this->*_opTable[opCode])();
		} else {
			std::cout << "Invalid opcode: " << std::hex << (short)opCode;
		}

		if(!_runIRQ && opCode == 0x40 && _state.IRQFlag > 0 && !CheckFlag(PSFlags::Interrupt)) {
			//"If an IRQ is pending and an RTI is executed that clears the I flag, the CPU will invoke the IRQ handler immediately after RTI finishes executing."
			_runIRQ = true;
		}
	} else {
		if(_runNMI) {
			NMI();
			_runNMI = false;
			_state.NMIFlag = false;
		} else if(_runIRQ) {
			IRQ();
		}
		_runIRQ = false;
	}

	return _cycleCount;
}

void CPU::EndFrame()
{
	_relativeCycleCount += _cycleCount;
	_cycleCount = 0;
}

void CPU::IncCycleCount()
{
	PPU::ExecStatic();
	APU::ExecStatic();
	_cycleCount++;
}

void CPU::RunDMATransfer(uint8_t* spriteRAM, uint32_t &spriteRamAddr, uint8_t offsetValue)
{
	//"the DMA procedure takes 513 CPU cycles (+1 on odd CPU cycles)"
	if((CPU::GetRelativeCycleCount() + Instance->_cycleCount) % 2 != 0) {
		Instance->IncCycleCount();
	}
	Instance->IncCycleCount();

	//DMA transfer starts at SpriteRamAddr and wraps around
	for(int i = 0; i < 0x100; i++) {
		//Read value
		uint8_t readValue = Instance->MemoryRead(offsetValue * 0x100 + i);

		//Write to ram
		spriteRAM[(spriteRamAddr+i) & 0xFF] = readValue;
		Instance->IncCycleCount();
	}
}


void CPU::StreamState(bool saving)
{
	Stream<uint16_t>(_state.PC);
	Stream<uint8_t>(_state.SP);
	Stream<uint8_t>(_state.PS);
	Stream<uint8_t>(_state.A);
	Stream<uint8_t>(_state.X);
	Stream<uint8_t>(_state.Y);
		
	Stream<int32_t>(_cycleCount);
	Stream<bool>(_state.NMIFlag);
	Stream<uint32_t>(_state.IRQFlag);

	Stream<bool>(_runNMI);
	Stream<bool>(_runIRQ);

	Stream<int32_t>(_relativeCycleCount);
}