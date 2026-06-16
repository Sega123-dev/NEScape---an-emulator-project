#include "6502ricoh.hpp"
#include <cstdlib>
CPU cpu;
Bus bus;
Instruction table[256];

CPU::CPU()
{
    for (int i = 0; i < 256; i++)
    {
        table[i] = {"???", &CPU::nop, AddressingMode::IMPLIED, 2};
    }

    table[0x00] = {"BRK", &CPU::brk, AddressingMode::IMPLIED, 7};
    table[0xEA] = {"NOP", &CPU::nop, AddressingMode::IMPLIED, 2};
    table[0x40] = {"RTI", &CPU::rti, AddressingMode::IMPLIED, 6};

    table[0x2C] = {"BIT", &CPU::bitAbsolute, AddressingMode::ABSOLUTE, 4};
    table[0x24] = {"BIT", &CPU::bitZeroPage, AddressingMode::ZERO_PAGE, 3};

    table[0xEE] = {"INC", &CPU::incAbsolute, AddressingMode::ABSOLUTE, 6};
    table[0xFE] = {"INC", &CPU::incAbsoluteX, AddressingMode::ABSOLUTE_X, 7};
    table[0xE6] = {"INC", &CPU::incZeroPage, AddressingMode::ZERO_PAGE, 5};
    table[0xF6] = {"INC", &CPU::incZeroPageX, AddressingMode::ABSOLUTE, 6};

    table[0xE8] = {"INX", &CPU::inx, AddressingMode::IMPLIED, 2};
    table[0xC8] = {"INY", &CPU::iny, AddressingMode::IMPLIED, 2};

    table[0xCE] = {"DEC", &CPU::decAbsolute, AddressingMode::ABSOLUTE, 6};
    table[0xDE] = {"DEC", &CPU::decAbsoluteX, AddressingMode::ABSOLUTE_X, 7};
    table[0xC6] = {"DEC", &CPU::decZeroPage, AddressingMode::ZERO_PAGE, 5};
    table[0xD6] = {"DEC", &CPU::decZeroPageX, AddressingMode::ABSOLUTE, 6};

    table[0xCA] = {"DEX", &CPU::dex, AddressingMode::IMPLIED, 2};
    table[0x88] = {"DEY", &CPU::dey, AddressingMode::IMPLIED, 2};

    table[0x18] = {"CLC", &CPU::clc, AddressingMode::IMPLIED, 2};
    table[0x38] = {"SEC", &CPU::sec, AddressingMode::IMPLIED, 2};

    table[0x78] = {"SEI", &CPU::sei, AddressingMode::IMPLIED, 2};
    table[0x58] = {"CLI", &CPU::cli, AddressingMode::IMPLIED, 2};

    table[0xB8] = {"CLV", &CPU::clv, AddressingMode::IMPLIED, 2};

    table[0xA9] = {"LDA", &CPU::ldaImmediate, AddressingMode::IMMEDIATE, 2};
    table[0xA5] = {"LDA", &CPU::ldaZeroPage, AddressingMode::ZERO_PAGE, 3};
    table[0xB5] = {"LDA", &CPU::ldaZeroPageX, AddressingMode::ZERO_PAGE_X, 4};
    table[0xAD] = {"LDA", &CPU::ldaAbsolute, AddressingMode::ABSOLUTE, 4};
    table[0xBD] = {"LDA", &CPU::ldaAbsoluteX, AddressingMode::ABSOLUTE_X, 4};
    table[0xB9] = {"LDA", &CPU::ldaAbsoluteY, AddressingMode::ABSOLUTE_Y, 4};
    table[0xA1] = {"LDA", &CPU::ldaIndexedIndirect, AddressingMode::INDEXED_INDIRECT, 6};
    table[0xB1] = {"LDA", &CPU::ldaIndirectIndexed, AddressingMode::INDIRECT_INDEXED, 5};

    table[0xA2] = {"LDX", &CPU::ldxImmediate, AddressingMode::IMMEDIATE, 2};
    table[0xA6] = {"LDX", &CPU::ldxZeroPage, AddressingMode::ZERO_PAGE, 3};
    table[0xB6] = {"LDX", &CPU::ldxZeroPageY, AddressingMode::ZERO_PAGE_Y, 4};
    table[0xAE] = {"LDX", &CPU::ldxAbsolute, AddressingMode::ABSOLUTE, 4};
    table[0xBE] = {"LDX", &CPU::ldxAbsoluteY, AddressingMode::ABSOLUTE_Y, 4};

    table[0xA0] = {"LDY", &CPU::ldyImmediate, AddressingMode::IMMEDIATE, 2};
    table[0xA4] = {"LDY", &CPU::ldyZeroPage, AddressingMode::ZERO_PAGE, 3};
    table[0xB4] = {"LDY", &CPU::ldyZeroPageX, AddressingMode::ZERO_PAGE_X, 4};
    table[0xAC] = {"LDY", &CPU::ldyAbsolute, AddressingMode::ABSOLUTE, 4};
    table[0xBC] = {"LDY", &CPU::ldyAbsoluteX, AddressingMode::ABSOLUTE_X, 4};

    table[0x85] = {"STA", &CPU::staZeroPage, AddressingMode::ZERO_PAGE, 3};
    table[0x95] = {"STA", &CPU::staZeroPageX, AddressingMode::ZERO_PAGE_X, 4};
    table[0x8D] = {"STA", &CPU::staAbsolute, AddressingMode::ABSOLUTE, 4};
    table[0x9D] = {"STA", &CPU::staAbsoluteX, AddressingMode::ABSOLUTE_X, 5};
    table[0x99] = {"STA", &CPU::staAbsoluteY, AddressingMode::ABSOLUTE_Y, 5};
    table[0x81] = {"STA", &CPU::staIndexedIndirect, AddressingMode::INDEXED_INDIRECT, 6};
    table[0x91] = {"STA", &CPU::staIndirectIndexed, AddressingMode::INDIRECT_INDEXED, 6};

    table[0x86] = {"STX", &CPU::stxZeroPage, AddressingMode::ZERO_PAGE, 3};
    table[0x96] = {"STX", &CPU::stxZeroPageY, AddressingMode::ZERO_PAGE_Y, 4};
    table[0x8E] = {"STX", &CPU::stxAbsolute, AddressingMode::ABSOLUTE, 4};

    table[0x84] = {"STY", &CPU::styZeroPage, AddressingMode::ZERO_PAGE, 3};
    table[0x94] = {"STY", &CPU::styZeroPageX, AddressingMode::ZERO_PAGE_Y, 4};
    table[0x8C] = {"STY", &CPU::styAbsolute, AddressingMode::ABSOLUTE, 4};

    table[0xAA] = {"TAX", &CPU::tax, AddressingMode::IMPLIED, 2};
    table[0xA8] = {"TAY", &CPU::tay, AddressingMode::IMPLIED, 2};
    table[0x8A] = {"TXA", &CPU::txa, AddressingMode::IMPLIED, 2};
    table[0x98] = {"TYA", &CPU::tya, AddressingMode::IMPLIED, 2};

    table[0xBA] = {"TSX", &CPU::tsx, AddressingMode::IMPLIED, 2};
    table[0x9A] = {"TXS", &CPU::txs, AddressingMode::IMPLIED, 2};

    table[0x48] = {"PHA", &CPU::pha, AddressingMode::IMPLIED, 3};
    table[0x68] = {"PLA", &CPU::pla, AddressingMode::IMPLIED, 4};
    table[0x08] = {"PHP", &CPU::php, AddressingMode::IMPLIED, 3};
    table[0x28] = {"PLP", &CPU::plp, AddressingMode::IMPLIED, 4};

    table[0x4C] = {"JMP", &CPU::jmpAbsolute, AddressingMode::ABSOLUTE, 3};
    table[0x6C] = {"JMP", &CPU::jmpIndirect, AddressingMode::INDIRECT, 5};

    table[0x20] = {"JSR", &CPU::jsrAbsolute, AddressingMode::ABSOLUTE, 6};
    table[0x60] = {"RTS", &CPU::rts, AddressingMode::IMPLIED, 6};

    table[0x69] = {"ADC", &CPU::adcImmediate, AddressingMode::IMMEDIATE, 2};
    table[0x65] = {"ADC", &CPU::adcZeroPage, AddressingMode::ZERO_PAGE, 3};
    table[0x75] = {"ADC", &CPU::adcZeroPageX, AddressingMode::ZERO_PAGE_X, 4};
    table[0x6D] = {"ADC", &CPU::adcAbsolute, AddressingMode::ABSOLUTE, 4};
    table[0x7D] = {"ADC", &CPU::adcAbsoluteX, AddressingMode::ABSOLUTE_X, 4};
    table[0x79] = {"ADC", &CPU::adcAbsoluteY, AddressingMode::ABSOLUTE_Y, 4};
    table[0x61] = {"ADC", &CPU::adcIndexedIndirect, AddressingMode::INDEXED_INDIRECT, 6};
    table[0x71] = {"ADC", &CPU::adcIndirectIndexed, AddressingMode::INDIRECT_INDEXED, 5};

    table[0xE9] = {"SBC", &CPU::sbcImmediate, AddressingMode::IMMEDIATE, 2};
    table[0xE5] = {"SBC", &CPU::sbcZeroPage, AddressingMode::ZERO_PAGE, 3};
    table[0xF5] = {"SBC", &CPU::sbcZeroPageX, AddressingMode::ZERO_PAGE_X, 4};
    table[0xED] = {"SBC", &CPU::sbcAbsolute, AddressingMode::ABSOLUTE, 4};
    table[0xFD] = {"SBC", &CPU::sbcAbsoluteX, AddressingMode::ABSOLUTE_X, 4};
    table[0xF9] = {"SBC", &CPU::sbcAbsoluteY, AddressingMode::ABSOLUTE_Y, 4};
    table[0xE1] = {"SBC", &CPU::sbcIndexedIndirect, AddressingMode::INDEXED_INDIRECT, 6};
    table[0xF1] = {"SBC", &CPU::sbcIndirectIndexed, AddressingMode::INDIRECT_INDEXED, 5};

    table[0x90] = {"BCC", &CPU::bcc, AddressingMode::RELATIVE, 2};
    table[0xB0] = {"BCS", &CPU::bcs, AddressingMode::RELATIVE, 2};
    table[0xF0] = {"BEQ", &CPU::beq, AddressingMode::RELATIVE, 2};
    table[0xD0] = {"BNE", &CPU::bne, AddressingMode::RELATIVE, 2};
    table[0x30] = {"BMI", &CPU::bmi, AddressingMode::RELATIVE, 2};
    table[0x10] = {"BPL", &CPU::bpl, AddressingMode::RELATIVE, 2};
    table[0x50] = {"BVC", &CPU::bvc, AddressingMode::RELATIVE, 2};
    table[0x70] = {"BVS", &CPU::bvs, AddressingMode::RELATIVE, 2};

    table[0x29] = {"AND", &CPU::andImmediate, AddressingMode::IMMEDIATE, 2};
    table[0x25] = {"AND", &CPU::andZeroPage, AddressingMode::ZERO_PAGE, 3};
    table[0x35] = {"AND", &CPU::andZeroPageX, AddressingMode::ZERO_PAGE_X, 4};
    table[0x2D] = {"AND", &CPU::andAbsolute, AddressingMode::ABSOLUTE, 4};
    table[0x3D] = {"AND", &CPU::andAbsoluteX, AddressingMode::ABSOLUTE_X, 4};
    table[0x39] = {"AND", &CPU::andAbsoluteY, AddressingMode::ABSOLUTE_Y, 4};
    table[0x21] = {"AND", &CPU::andIndexedIndirect, AddressingMode::INDEXED_INDIRECT, 6};
    table[0x31] = {"AND", &CPU::andIndirectIndexed, AddressingMode::INDIRECT_INDEXED, 5};

    table[0x49] = {"EOR", &CPU::eorImmediate, AddressingMode::IMMEDIATE, 2};
    table[0x45] = {"EOR", &CPU::eorZeroPage, AddressingMode::ZERO_PAGE, 3};
    table[0x55] = {"EOR", &CPU::eorZeroPageX, AddressingMode::ZERO_PAGE_X, 4};
    table[0x4D] = {"EOR", &CPU::eorAbsolute, AddressingMode::ABSOLUTE, 4};
    table[0x5D] = {"EOR", &CPU::eorAbsoluteX, AddressingMode::ABSOLUTE_X, 4};
    table[0x59] = {"EOR", &CPU::eorAbsoluteY, AddressingMode::ABSOLUTE_Y, 4};
    table[0x41] = {"EOR", &CPU::eorIndexedIndirect, AddressingMode::INDEXED_INDIRECT, 6};
    table[0x51] = {"EOR", &CPU::eorIndirectIndexed, AddressingMode::INDIRECT_INDEXED, 5};

    table[0x09] = {"ORA", &CPU::oraImmediate, AddressingMode::IMMEDIATE, 2};
    table[0x05] = {"ORA", &CPU::oraZeroPage, AddressingMode::ZERO_PAGE, 3};
    table[0x15] = {"ORA", &CPU::oraZeroPageX, AddressingMode::ZERO_PAGE_X, 4};
    table[0x0D] = {"ORA", &CPU::oraAbsolute, AddressingMode::ABSOLUTE, 4};
    table[0x1D] = {"ORA", &CPU::oraAbsoluteX, AddressingMode::ABSOLUTE_X, 4};
    table[0x19] = {"ORA", &CPU::oraAbsoluteY, AddressingMode::ABSOLUTE_Y, 4};
    table[0x01] = {"ORA", &CPU::oraIndexedIndirect, AddressingMode::INDEXED_INDIRECT, 6};
    table[0x11] = {"ORA", &CPU::oraIndirectIndexed, AddressingMode::INDIRECT_INDEXED, 5};

    table[0xC9] = {"CMP", &CPU::cmpImmediate, AddressingMode::IMMEDIATE, 2};
    table[0xC5] = {"CMP", &CPU::cmpZeroPage, AddressingMode::ZERO_PAGE, 3};
    table[0xD5] = {"CMP", &CPU::cmpZeroPageX, AddressingMode::ZERO_PAGE_X, 4};
    table[0xCD] = {"CMP", &CPU::cmpAbsolute, AddressingMode::ABSOLUTE, 4};
    table[0xDD] = {"CMP", &CPU::cmpAbsoluteX, AddressingMode::ABSOLUTE_X, 4};
    table[0xD9] = {"CMP", &CPU::cmpAbsoluteY, AddressingMode::ABSOLUTE_Y, 4};
    table[0xC1] = {"CMP", &CPU::cmpIndexedIndirect, AddressingMode::INDEXED_INDIRECT, 6};
    table[0xD1] = {"CMP", &CPU::cmpIndirectIndexed, AddressingMode::INDIRECT_INDEXED, 5};

    table[0xE0] = {"CPX", &CPU::cpxImmediate, AddressingMode::IMMEDIATE, 2};
    table[0xE4] = {"CPX", &CPU::cpxZeroPage, AddressingMode::ZERO_PAGE, 3};
    table[0xEC] = {"CPX", &CPU::cpxAbsolute, AddressingMode::ABSOLUTE, 4};

    table[0xC0] = {"CPY", &CPU::cpyImmediate, AddressingMode::IMMEDIATE, 2};
    table[0xC4] = {"CPY", &CPU::cpyZeroPage, AddressingMode::ZERO_PAGE, 3};
    table[0xCC] = {"CPY", &CPU::cpyAbsolute, AddressingMode::ABSOLUTE, 4};

    table[0x0A] = {"ASL", &CPU::aslAccumulator, AddressingMode::ACCUMULATOR, 2};
    table[0x06] = {"ASL", &CPU::aslZeroPage, AddressingMode::ZERO_PAGE, 5};
    table[0x16] = {"ASL", &CPU::aslZeroPageX, AddressingMode::ZERO_PAGE_X, 6};
    table[0x0E] = {"ASL", &CPU::aslAbsolute, AddressingMode::ABSOLUTE, 6};
    table[0x1E] = {"ASL", &CPU::aslAbsoluteX, AddressingMode::ABSOLUTE_X, 7};

    table[0x4A] = {"LSR", &CPU::lsrAccumulator, AddressingMode::ACCUMULATOR, 2};
    table[0x46] = {"LSR", &CPU::lsrZeroPage, AddressingMode::ZERO_PAGE, 5};
    table[0x56] = {"LSR", &CPU::lsrZeroPageX, AddressingMode::ZERO_PAGE_X, 6};
    table[0x4E] = {"LSR", &CPU::lsrAbsolute, AddressingMode::ABSOLUTE, 6};
    table[0x5E] = {"LSR", &CPU::lsrAbsoluteX, AddressingMode::ABSOLUTE_X, 7};

    table[0x6A] = {"ROR", &CPU::rorAccumulator, AddressingMode::ACCUMULATOR, 2};
    table[0x66] = {"ROR", &CPU::rorZeroPage, AddressingMode::ZERO_PAGE, 5};
    table[0x76] = {"ROR", &CPU::rorZeroPageX, AddressingMode::ZERO_PAGE_X, 6};
    table[0x6E] = {"ROR", &CPU::rorAbsolute, AddressingMode::ABSOLUTE, 6};
    table[0x7E] = {"ROR", &CPU::rorAbsoluteX, AddressingMode::ABSOLUTE_X, 7};

    table[0x2A] = {"ROL", &CPU::rolAccumulator, AddressingMode::ACCUMULATOR, 2};
    table[0x26] = {"ROL", &CPU::rolZeroPage, AddressingMode::ZERO_PAGE, 5};
    table[0x36] = {"ROL", &CPU::rolZeroPageX, AddressingMode::ZERO_PAGE_X, 6};
    table[0x2E] = {"ROL", &CPU::rolAbsolute, AddressingMode::ABSOLUTE, 6};
    table[0x3E] = {"ROL", &CPU::rolAbsoluteX, AddressingMode::ABSOLUTE_X, 7};
};
// HELPERS

void CPU::pushPC()
{
    bus.write(0x0100 + sp, pc & 0xFF); // low
    sp--;
    bus.write(0x0100 + sp, (pc >> 8) & 0xFF); // high
    sp--;
}
void CPU::pushStatus(bool breakFlag)
{
    uint8_t flags = P;

    if (breakFlag)
        flags |= 0x10;
    else
        flags &= ~0x10;

    flags |= 0x20;

    bus.write(0x0100 + sp, flags);
    sp--;
}
void CPU::setNZ(uint8_t reg)
{
    P &= ~(0x80 | 0x02);

    if (reg & 0x80)
        P |= 0x80;

    if (reg == 0)
        P |= 0x02;
}

void CPU::setVC(uint8_t V, uint8_t C)
{
    if (C)
        P |= 0x01;
    else
        P &= ~0x01;

    if (V)
        P |= 0x40;
    else
        P &= ~0x40;
}

void CPU::compare(uint8_t reg, uint8_t value)
{
    uint16_t result = reg - value;

    if (reg >= value)
        P |= 0x01;
    else
        P &= ~0x01;
    if ((result & 0xFF) == 0)
        P |= 0x02;
    else
        P &= ~0x02;

    if (result & 0x80)
        P |= 0x80;
    else
        P &= ~0x80;
}
void CPU::setIRQ()
{
    IRQ = true;
}
void CPU::clearIRQ()
{
    IRQ = false;
}
// REST OF THE CPU

void CPU::reset()
{

    NMI = IRQ = RESET = false;
    A = X = Y = 0;
    P = 0x24;
    sp = 0xFD;
    cycles = 0;
    opState = 0;
    pc = 0;

    uint8_t low = bus.read(0xFFFC);
    uint8_t high = bus.read(0xFFFD);
    pc = (high << 8) | low;
    std::cout << "Reset Vector: " << std::hex << pc << "\n";
}
void CPU::brk()
{
    uint16_t returnAddr = pc + 1;

    bus.write(0x0100 + sp, (returnAddr >> 8) & 0xFF);
    sp--;

    bus.write(0x0100 + sp, returnAddr & 0xFF);
    sp--;

    uint8_t flags = P | 0x30;
    bus.write(0x0100 + sp, flags);
    sp--;

    P |= 0x04;

    uint8_t lowByte = bus.read(0xFFFE);
    uint8_t highByte = bus.read(0xFFFF);

    pc = (highByte << 8) | lowByte;
}

void CPU::nop()
{
    pc++;
}
void CPU::rti()
{
    sp++;
    uint8_t status = bus.read(0x0100 + sp);

    sp++;
    uint8_t low = bus.read(0x0100 + sp);

    sp++;
    uint8_t high = bus.read(0x0100 + sp);

    P = status;
    P &= ~0x10;
    P |= 0x20;

    pc = (high << 8) | low;
}
void CPU::bitAbsolute()
{
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);

    uint16_t addr = (highByte << 8) | lowByte;
    uint8_t value = bus.read(addr);

    uint8_t result = A & value;

    setNZ(result);
}
void CPU::bitZeroPage()
{
    uint8_t addr = bus.read(pc++);
    uint8_t value = bus.read(addr);

    uint8_t result = A & value;

    setNZ(result);
}
void CPU::incAbsolute()
{
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);
    uint16_t finalAddress = (highByte << 8) | lowByte;

    uint16_t value = bus.read(finalAddress) + 1;
    uint8_t bit8val = value & 0xFF;

    bus.write(finalAddress, bit8val);

    setNZ(bit8val);
}
void CPU::incAbsoluteX()
{
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);
    uint16_t finalAddress = (highByte << 8) | lowByte;

    uint16_t value = bus.read(finalAddress + X) + 1;
    uint8_t bit8val = value & 0xFF;

    bus.write(finalAddress + X, bit8val);

    setNZ(bit8val);
}
void CPU::incZeroPage()
{
    uint8_t address = bus.read(pc++);
    uint8_t bit8val = bus.read(address);

    bus.write(address, bit8val + 1);

    setNZ(bit8val + 1);
}
void CPU::incZeroPageX()
{
    uint8_t address = bus.read(pc++);
    uint8_t bit8val = bus.read((address + X) & 0xFF);

    bus.write((address + X) & 0xFF, bit8val + 1);

    setNZ(bit8val + 1);
}
void CPU::inx()
{
    X++;

    setNZ(X);
}
void CPU::iny()
{
    Y++;

    setNZ(Y);
}
void CPU::decAbsolute()
{
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);
    uint16_t finalAddress = (highByte << 8) | lowByte;

    uint16_t value = bus.read(finalAddress) - 1;
    uint8_t bit8val = value & 0xFF;

    bus.write(finalAddress, bit8val);

    setNZ(bit8val);
}
void CPU::decAbsoluteX()
{
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);
    uint16_t finalAddress = (highByte << 8) | lowByte;

    uint16_t value = bus.read(finalAddress + X) - 1;
    uint8_t bit8val = value & 0xFF;

    bus.write(finalAddress + X, bit8val);

    setNZ(bit8val);
}
void CPU::decZeroPage()
{
    uint8_t address = bus.read(pc++);
    uint8_t bit8val = bus.read(address);

    bus.write(address, bit8val - 1);

    setNZ(bit8val - 1);
}
void CPU::decZeroPageX()
{
    uint8_t address = bus.read(pc++);
    uint8_t bit8val = bus.read((address + X) & 0xFF);

    bus.write((address + X) & 0xFF, bit8val - 1);

    setNZ(bit8val - 1);
}
void CPU::dex()
{
    X--;

    setNZ(X);
}
void CPU::dey()
{
    Y--;

    setNZ(Y);
}
void CPU::clc()
{
    P &= ~0x01;
}
void CPU::sec()
{
    P |= 0x01;
}

void CPU::cli()
{
    P &= ~0x04;
}
void CPU::sei()
{
    P |= 0x04;
}
void CPU::clv()
{
    P &= ~0x40;
}
void CPU::ldaImmediate()
{
    uint8_t value = bus.read(pc++);
    A = value;

    setNZ(A);
}
void CPU::ldaZeroPage()
{
    uint8_t address = bus.read(pc++);
    uint8_t value = bus.read(address);

    A = value;

    setNZ(A);
}
void CPU::ldaZeroPageX()
{
    uint8_t address = bus.read(pc++);
    uint8_t zeroPageAddress = (address + X) & 0xFF;
    uint8_t value = bus.read(zeroPageAddress);

    A = value;

    setNZ(A);
}
void CPU::ldaAbsolute()
{
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);
    uint16_t finalAddress = (highByte << 8) | lowByte;
    uint8_t value = bus.read(finalAddress);

    A = value;
    setNZ(A);
}
void CPU::ldaAbsoluteX()
{
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);
    uint16_t finalAddress = (highByte << 8) | lowByte;
    uint16_t finalAddressX = finalAddress + X;
    uint8_t value = bus.read(finalAddressX);

    A = value;

    if ((finalAddress & 0xFF00) != (finalAddressX & 0xFF00))
        cycles++;

    A = value;
    setNZ(A);
}
void CPU::ldaAbsoluteY()
{
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);
    uint16_t finalAddress = (highByte << 8) | lowByte;
    uint16_t finalAddressY = finalAddress + Y;
    uint8_t value = bus.read(finalAddressY);

    A = value;

    if ((finalAddress & 0xFF00) != (finalAddressY & 0xFF00))
        cycles++;

    setNZ(A);
}
void CPU::ldaIndexedIndirect()
{
    uint8_t address = bus.read(pc++);
    uint8_t zeroPageAddress = (address + X) & 0xFF;
    uint8_t lowByte = bus.read(zeroPageAddress);
    uint8_t highByte = bus.read((zeroPageAddress + 1) & 0xFF);
    uint16_t ptr = (highByte << 8) | lowByte;
    A = bus.read(ptr);

    setNZ(A);
}
void CPU::ldaIndirectIndexed()
{
    uint8_t address = bus.read(pc++);
    uint8_t lowByte = bus.read(address);
    uint8_t highByte = bus.read((address + 1) & 0xFF);
    uint16_t ptr = (highByte << 8) | lowByte;
    uint16_t finalAddress = ptr + Y;
    A = bus.read(finalAddress);

    setNZ(A);
}
void CPU::ldxImmediate()
{
    uint8_t value = bus.read(pc++);
    X = value;

    setNZ(X);
}
void CPU::ldxZeroPage()
{
    uint8_t address = bus.read(pc++);
    uint8_t value = bus.read(address);

    X = value;

    setNZ(X);
}
void CPU::ldxZeroPageY()
{
    uint8_t address = bus.read(pc++);
    uint8_t zeroPageAddress = (address + Y) & 0xFF;
    uint8_t value = bus.read(zeroPageAddress);

    X = value;

    setNZ(X);
}
void CPU::ldxAbsolute()
{
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);
    uint16_t finalAddress = (highByte << 8) | lowByte;
    uint8_t value = bus.read(finalAddress);

    X = value;

    setNZ(X);
}
void CPU::ldxAbsoluteY()
{
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);
    uint16_t finalAddress = (highByte << 8) | lowByte;
    uint16_t finalAddressY = finalAddress + Y;
    uint8_t value = bus.read(finalAddressY);

    X = value;

    if ((finalAddress & 0xFF00) != (finalAddressY & 0xFF00))
        cycles++;

    setNZ(X);
}
void CPU::ldyImmediate()
{
    uint8_t value = bus.read(pc++);
    Y = value;

    setNZ(Y);
}
void CPU::ldyZeroPage()
{
    uint8_t address = bus.read(pc++);
    uint8_t value = bus.read(address);

    Y = value;

    setNZ(Y);
}
void CPU::ldyZeroPageX()
{
    uint8_t address = bus.read(pc++);
    uint8_t zeroPageAddress = (address + X) & 0xFF;
    uint8_t value = bus.read(zeroPageAddress);

    Y = value;

    setNZ(Y);
}
void CPU::ldyAbsolute()
{
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);
    uint16_t finalAddress = (highByte << 8) | lowByte;
    uint8_t value = bus.read(finalAddress);

    Y = value;

    setNZ(Y);
}
void CPU::ldyAbsoluteX()
{
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);
    uint16_t finalAddress = (highByte << 8) | lowByte;
    uint16_t finalAddressX = finalAddress + X;
    uint8_t value = bus.read(finalAddressX);

    Y = value;

    if ((finalAddress & 0xFF00) != (finalAddressX & 0xFF00))
        cycles++;

    setNZ(Y);
}
void CPU::staAbsolute()
{
    uint8_t lowByte = bus.read(pc);
    uint8_t highByte = bus.read(pc + 1);
    uint16_t finalAddress = (highByte << 8) | lowByte;
    pc += 2;
    bus.write(finalAddress, A);
}
void CPU::staAbsoluteX()
{
    uint8_t lowByte = bus.read(pc);
    uint8_t highByte = bus.read(pc + 1);
    uint16_t finalAddress = (highByte << 8) | lowByte;
    pc += 2;
    bus.write(finalAddress + X, A);
}
void CPU::staAbsoluteY()
{
    uint8_t lowByte = bus.read(pc);
    uint8_t highByte = bus.read(pc + 1);
    uint16_t finalAddress = (highByte << 8) | lowByte;
    pc += 2;
    bus.write(finalAddress + Y, A);
}
void CPU::staZeroPage()
{
    uint8_t address = bus.read(pc);
    pc += 1;
    bus.write(address, A);
}
void CPU::staZeroPageX()
{
    uint8_t address = bus.read(pc);
    pc += 1;
    bus.write((address + X) & 0xFF, A);
}
void CPU::staIndexedIndirect()
{
    uint8_t baseAddress = bus.read(pc);
    pc += 1;
    uint16_t ptr = (baseAddress + X) & 0xFF;
    uint8_t lowByte = bus.read(ptr);
    uint8_t highByte = bus.read((ptr + 1) & 0xFF);
    uint16_t finalAddress = (highByte << 8) | lowByte;

    bus.write(finalAddress, A);
}
void CPU::staIndirectIndexed()
{
    uint8_t baseAddress = bus.read(pc);
    pc += 1;
    uint8_t lowByte = bus.read(baseAddress);
    uint8_t highByte = bus.read((baseAddress + 1) & 0xFF);
    uint16_t finalAddress = (highByte << 8) | lowByte;
    finalAddress += Y;
    bus.write(finalAddress, A);
}
void CPU::stxAbsolute()
{
    uint8_t lowByte = bus.read(pc);
    uint8_t highByte = bus.read(pc + 1);
    uint16_t finalAddress = (highByte << 8) | lowByte;
    pc += 2;
    bus.write(finalAddress, X);
}
void CPU::stxZeroPage()
{
    uint8_t address = bus.read(pc);
    pc += 1;
    bus.write(address, X);
}
void CPU::stxZeroPageY()
{
    uint8_t baseAddr = bus.read(pc);
    pc += 1;
    bus.write((baseAddr + Y) & 0xFF, X);
}
void CPU::styAbsolute()
{
    uint8_t lowByte = bus.read(pc);
    uint8_t highByte = bus.read(pc + 1);
    uint16_t finalAddress = (highByte << 8) | lowByte;
    pc += 2;
    bus.write(finalAddress, Y);
}
void CPU::styZeroPage()
{
    uint8_t address = bus.read(pc);
    pc += 1;
    bus.write(address, Y);
}
void CPU::styZeroPageX()
{
    uint8_t baseAddr = bus.read(pc);
    pc += 1;
    bus.write((baseAddr + X) & 0xFF, Y);
}
void CPU::tax()
{
    X = A;
    setNZ(X);
}
void CPU::tay()
{
    Y = A;
    setNZ(Y);
}
void CPU::txa()
{
    A = X;
    setNZ(A);
}
void CPU::tya()
{
    A = Y;
    setNZ(A);
}
void CPU::tsx()
{
    X = sp;
    setNZ(X);
}
void CPU::txs()
{
    sp = X;
}
void CPU::pha()
{
    bus.write(0x0100 + sp, A);
    sp--;
    cycles++;
}
void CPU::pla()
{
    sp++;
    A = bus.read(0x0100 + sp);

    setNZ(A);
    cycles++;
}
void CPU::php()
{
    bus.write(0x0100 + sp, P | 0x30);
    sp--;
}
void CPU::plp()
{
    sp++;
    P = bus.read(0x0100 + sp);

    P &= 0xEF;
    P |= 0x20;
}
void CPU::jmpAbsolute()
{
    uint8_t lowByte = bus.read(pc);
    uint8_t highByte = bus.read(pc + 1);
    uint16_t finalAddress = (highByte << 8) | lowByte;

    pc = finalAddress;
}
void CPU::jmpIndirect()
{
    uint16_t pointer = bus.read(pc) | (bus.read(pc + 1) << 8);
    pc += 2;

    uint8_t lowByte = bus.read(pointer);

    uint8_t highByte;
    if ((pointer & 0x00FF) == 0x00FF)
        highByte = bus.read(pointer & 0xFF00);
    else
        highByte = bus.read(pointer + 1);

    pc = (highByte << 8) | lowByte;
}
void CPU::jsrAbsolute()
{
    uint16_t addr = bus.read(pc) | (bus.read(pc + 1) << 8);
    pc += 2;

    uint16_t returnAddress = pc - 1;

    bus.write(0x0100 + sp, (returnAddress >> 8) & 0xFF);
    sp--;
    bus.write(0x0100 + sp, returnAddress & 0xFF);
    sp--;

    pc = addr;
    std::cout << "Final addr JSR: " << pc << std::endl;
}
void CPU::rts()
{
    sp++;

    uint8_t lowByte = bus.read(0x0100 + sp);
    sp++;

    uint8_t highByte = bus.read(0x0100 + sp);

    uint16_t addr = (highByte << 8) | lowByte;

    pc = addr + 1;
}
void CPU::adcImmediate()
{
    uint8_t C, V = 0;
    uint8_t operand = bus.read(pc++);
    uint16_t result = A + operand + (P & 0x01);
    uint8_t newRes = result & 0xFF;
    C = (result > 0xFF) ? 1 : 0;
    V = (~(A ^ operand) & (A ^ newRes) & 0x80) ? 1 : 0;

    setVC(V, C);

    A = newRes;

    setNZ(A);
}
void CPU::adcZeroPage()
{
    uint8_t C, V = 0;
    uint8_t zpAddr = bus.read(pc++);
    uint8_t operand = bus.read(zpAddr);
    uint16_t result = A + operand + (P & 0x01);
    uint8_t newRes = result & 0xFF;
    C = (result > 0xFF) ? 1 : 0;
    V = (~(A ^ operand) & (A ^ newRes) & 0x80) ? 1 : 0;

    setVC(V, C);

    A = newRes;

    setNZ(A);
}
void CPU::adcZeroPageX()
{
    uint8_t C, V = 0;
    uint8_t baseAddr = bus.read(pc++);
    uint8_t zpAddr = (baseAddr + X) & 0xFF;
    uint8_t operand = bus.read(zpAddr);
    uint16_t result = A + operand + (P & 0x01);
    uint8_t newRes = result & 0xFF;
    C = (result > 0xFF) ? 1 : 0;
    V = (~(A ^ operand) & (A ^ newRes) & 0x80) ? 1 : 0;

    setVC(V, C);

    A = newRes;

    setNZ(A);
}
void CPU::adcAbsolute()
{
    uint8_t C, V = 0;
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);
    uint16_t baseAddr = (highByte << 8) | lowByte;
    uint8_t operand = bus.read(baseAddr);

    uint16_t result = A + operand + (P & 0x01);
    uint8_t newRes = result & 0xFF;

    C = (result > 0xFF) ? 1 : 0;
    V = (~(A ^ operand) & (A ^ newRes) & 0x80) ? 1 : 0;

    setVC(V, C);

    A = newRes;

    setNZ(A);
}
void CPU::adcAbsoluteX()
{
    uint8_t C, V = 0;
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);
    uint16_t baseAddr = ((highByte << 8) | lowByte) + X;
    uint8_t operand = bus.read(baseAddr);

    if ((baseAddr & 0xFF00) != (lowByte << 8))
        cycles++;

    uint16_t result = A + operand + (P & 0x01);
    uint8_t newRes = result & 0xFF;

    C = (result > 0xFF) ? 1 : 0;
    V = (~(A ^ operand) & (A ^ newRes) & 0x80) ? 1 : 0;

    setVC(V, C);

    A = newRes;

    setNZ(A);
}
void CPU::adcAbsoluteY()
{
    uint8_t C, V = 0;
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);
    uint16_t baseAddr = ((highByte << 8) | lowByte) + Y;
    uint8_t operand = bus.read(baseAddr);

    if ((baseAddr & 0xFF00) != (lowByte << 8))
        cycles++;

    uint16_t result = A + operand + (P & 0x01);
    uint8_t newRes = result & 0xFF;

    C = (result > 0xFF) ? 1 : 0;
    V = (~(A ^ operand) & (A ^ newRes) & 0x80) ? 1 : 0;

    setVC(V, C);

    A = newRes;

    setNZ(A);
}
void CPU::adcIndexedIndirect()
{
    uint8_t C, V = 0;
    uint8_t zp = bus.read(pc++);
    uint8_t ptr = (zp + X) & 0xFF;
    uint8_t lowByte = bus.read(ptr);
    uint8_t highByte = bus.read((ptr + 1) & 0xFF);

    uint16_t effAddress = (highByte << 8) | lowByte;

    uint8_t operand = bus.read(effAddress);

    uint16_t result = A + operand + (P & 0x01);
    uint8_t newRes = result & 0xFF;

    C = (result > 0xFF) ? 1 : 0;
    V = (~(A ^ operand) & (A ^ newRes) & 0x80) ? 1 : 0;

    setVC(V, C);

    A = newRes;

    setNZ(A);
}
void CPU::adcIndirectIndexed()
{
    uint8_t C, V = 0;
    uint8_t zp = bus.read(pc++);
    uint8_t lowByte = bus.read(zp);
    uint8_t highByte = bus.read((zp + 1) & 0xFF);

    uint16_t effAddress = ((highByte << 8) | lowByte) + Y;

    if ((effAddress & 0xFF00) != (lowByte << 8))
        cycles++;

    uint8_t operand = bus.read(effAddress);

    uint16_t result = A + operand + (P & 0x01);
    uint8_t newRes = result & 0xFF;

    C = (result > 0xFF) ? 1 : 0;
    V = (~(A ^ operand) & (A ^ newRes) & 0x80) ? 1 : 0;

    setVC(V, C);

    A = newRes;

    setNZ(A);
}
void CPU::sbcImmediate()
{
    uint8_t value = bus.read(pc++);

    uint16_t temp = (uint16_t)A - value - (1 - (P & 0x01));

    if (temp < 0x100)
        P |= 0x01;
    else
        P &= ~0x01;

    if (((A ^ temp) & 0x80) && ((A ^ value) & 0x80))
        P |= 0x40;
    else
        P &= ~0x40;

    A = temp & 0xFF;

    setNZ(A);
}
void CPU::sbcZeroPage()
{
    uint8_t addr = bus.read(pc++);
    uint8_t value = bus.read(addr);
    uint16_t temp = (uint16_t)A - value - (1 - (P & 0x01));

    if (temp < 0x100)
        P |= 0x01;
    else
        P &= ~0x01;

    if (((A ^ temp) & 0x80) && ((A ^ value) & 0x80))
        P |= 0x40;
    else
        P &= ~0x40;

    A = temp & 0xFF;

    setNZ(A);
}
void CPU::sbcZeroPageX()
{
    uint8_t addr = bus.read(pc++);
    uint8_t value = bus.read(addr + X) & 0xFF;
    uint16_t temp = (uint16_t)A - value - (1 - (P & 0x01));

    if (temp < 0x100)
        P |= 0x01;
    else
        P &= ~0x01;

    if (((A ^ temp) & 0x80) && ((A ^ value) & 0x80))
        P |= 0x40;
    else
        P &= ~0x40;

    A = temp & 0xFF;

    setNZ(A);
}
void CPU::sbcAbsolute()
{
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);
    uint16_t baseAddr = ((highByte << 8) | lowByte);
    uint8_t value = bus.read(baseAddr);
    uint16_t temp = (uint16_t)A - value - (1 - (P & 0x01));

    if (temp < 0x100)
        P |= 0x01;
    else
        P &= ~0x01;

    if (((A ^ temp) & 0x80) && ((A ^ value) & 0x80))
        P |= 0x40;
    else
        P &= ~0x40;

    A = temp & 0xFF;

    setNZ(A);
}
void CPU::sbcAbsoluteX()
{
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);
    uint16_t baseAddr = ((highByte << 8) | lowByte);

    if ((baseAddr & 0xFF00) != ((baseAddr + X) & 0xFF00))
        cycles++;

    uint8_t value = bus.read(baseAddr + X);
    uint16_t temp = (uint16_t)A - value - (1 - (P & 0x01));

    if (temp < 0x100)
        P |= 0x01;
    else
        P &= ~0x01;

    if (((A ^ temp) & 0x80) && ((A ^ value) & 0x80))
        P |= 0x40;
    else
        P &= ~0x40;

    A = temp & 0xFF;

    setNZ(A);
}
void CPU::sbcAbsoluteY()
{
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);
    uint16_t baseAddr = ((highByte << 8) | lowByte);

    if ((baseAddr & 0xFF00) != ((baseAddr + Y) & 0xFF00))
        cycles++;

    uint8_t value = bus.read(baseAddr + Y);
    uint16_t temp = (uint16_t)A - value - (1 - (P & 0x01));

    if (temp < 0x100)
        P |= 0x01;
    else
        P &= ~0x01;

    if (((A ^ temp) & 0x80) && ((A ^ value) & 0x80))
        P |= 0x40;
    else
        P &= ~0x40;

    A = temp & 0xFF;

    setNZ(A);
}
void CPU::sbcIndexedIndirect()
{
    uint8_t zp = bus.read(pc++);
    uint8_t ptr = (zp + X) & 0xFF;

    uint8_t lowByte = bus.read(ptr);
    uint8_t highByte = bus.read((ptr + 1) & 0xFF);

    uint16_t addr = ((highByte << 8) | lowByte);

    uint8_t value = bus.read(addr);
    uint16_t temp = (uint16_t)A - value - (1 - (P & 0x01));

    if (temp < 0x100)
        P |= 0x01;
    else
        P &= ~0x01;

    if (((A ^ temp) & 0x80) && ((A ^ value) & 0x80))
        P |= 0x40;
    else
        P &= ~0x40;

    A = temp & 0xFF;

    setNZ(A);
}
void CPU::sbcIndirectIndexed()
{
    uint8_t zp = bus.read(pc++);

    uint8_t lowByte = bus.read(zp);
    uint8_t highByte = bus.read((zp + 1) & 0xFF);

    uint16_t addr = ((highByte << 8) | lowByte);

    if ((addr & 0xFF00) != ((addr + Y) & 0xFF00))
        cycles++;

    uint8_t value = bus.read(addr + Y);
    uint16_t temp = (uint16_t)A - value - (1 - (P & 0x01));

    if (temp < 0x100)
        P |= 0x01;
    else
        P &= ~0x01;

    if (((A ^ temp) & 0x80) && ((A ^ value) & 0x80))
        P |= 0x40;
    else
        P &= ~0x40;

    A = temp & 0xFF;

    setNZ(A);
}
void CPU::bcc()
{
    int8_t offset = (int8_t)bus.read(pc++);

    if (!(P & 0x01))
    {
        uint16_t oldPC = pc;

        pc += offset;
        cycles++;

        if ((oldPC & 0xFF00) != (pc & 0xFF00))
            cycles++;
    }
}

void CPU::bcs()
{
    int8_t offset = (int8_t)bus.read(pc++);

    if (P & 0x01)
    {
        uint16_t oldPC = pc;

        pc += offset;
        cycles++;

        if ((oldPC & 0xFF00) != (pc & 0xFF00))
            cycles++;
    }
}

void CPU::beq()
{
    int8_t offset = (int8_t)bus.read(pc++);
    uint16_t oldPC = pc;
    if (P & 0x02)
    {
        pc += offset;
        cycles++;
    }

    if ((oldPC & 0xFF00) != (pc & 0xFF00))
        cycles++;
}

void CPU::bne()
{
    int8_t offset = (int8_t)bus.read(pc++);

    if (!(P & 0x02))
    {
        uint16_t oldPC = pc;

        pc += offset;
        cycles++;

        if ((oldPC & 0xFF00) != (pc & 0xFF00))
            cycles++;
    }
}

void CPU::bmi()
{
    int8_t offset = (int8_t)bus.read(pc++);

    if (P & 0x80)
    {
        uint16_t oldPC = pc;

        pc += offset;
        cycles++;

        if ((oldPC & 0xFF00) != (pc & 0xFF00))
            cycles++;
    }
}

void CPU::bpl()
{
    int8_t offset = (int8_t)bus.read(pc++);

    if (!(P & 0x80))
    {
        uint16_t oldPC = pc;

        pc += offset;
        cycles++;

        if ((oldPC & 0xFF00) != (pc & 0xFF00))
            cycles++;
    }
}

void CPU::bvc()
{
    int8_t offset = (int8_t)bus.read(pc++);

    if (!(P & 0x40))
    {
        uint16_t oldPC = pc;

        pc += offset;
        cycles++;

        if ((oldPC & 0xFF00) != (pc & 0xFF00))
            cycles++;
    }
}

void CPU::bvs()
{
    int8_t offset = (int8_t)bus.read(pc++);

    if (P & 0x40)
    {
        uint16_t oldPC = pc;

        pc += offset;
        cycles++;

        if ((oldPC & 0xFF00) != (pc & 0xFF00))
            cycles++;
    }
}
void CPU::andImmediate()
{
    uint8_t value = bus.read(pc++);
    A = A & value;
    setNZ(A);
}
void CPU::andZeroPage()
{
    uint8_t addr = bus.read(pc++);
    uint8_t value = bus.read(addr);

    A = A & value;
    setNZ(A);
}
void CPU::andZeroPageX()
{
    uint8_t addr = bus.read(pc++);
    uint8_t value = bus.read((addr + X) & 0xFF);
    A = A & value;
    setNZ(A);
}
void CPU::andAbsolute()
{
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);

    uint16_t finalAddr = (highByte << 8) | lowByte;

    uint8_t value = bus.read(finalAddr);
    A = A & value;
    setNZ(A);
}
void CPU::andAbsoluteX()
{
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);

    uint16_t finalAddr = (highByte << 8) | lowByte;

    if ((finalAddr & 0xFF00) != ((finalAddr + X) & 0xFF00))
        cycles++;

    uint8_t value = bus.read(finalAddr + X);
    A = A & value;
    setNZ(A);
}
void CPU::andAbsoluteY()
{
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);

    uint16_t finalAddr = (highByte << 8) | lowByte;

    if ((finalAddr & 0xFF00) != ((finalAddr + Y) & 0xFF00))
        cycles++;

    uint8_t value = bus.read(finalAddr + Y);
    A = A & value;
    setNZ(A);
}
void CPU::andIndexedIndirect()
{
    uint8_t zp = bus.read(pc++);
    uint8_t ptr = bus.read(zp + X) & 0xFF;
    uint8_t lowByte = bus.read(ptr);
    uint8_t highByte = bus.read((ptr + 1) & 0xFF);
    uint16_t finalAddr = (highByte << 8) | lowByte;

    uint8_t value = bus.read(finalAddr);
    A = A & value;
    setNZ(A);
}
void CPU::andIndirectIndexed()
{
    uint8_t zp = bus.read(pc++);
    uint8_t lowByte = bus.read(zp);
    uint8_t highByte = bus.read((zp + 1) & 0xFF);
    uint16_t finalAddr = (highByte << 8) | lowByte;

    if ((finalAddr & 0xFF00) != ((finalAddr + Y) & 0xFF00))
        cycles++;

    uint8_t value = bus.read(finalAddr + Y);
    A = A & value;
    setNZ(A);
}
void CPU::eorImmediate()
{
    uint8_t value = bus.read(pc++);
    A = A ^ value;
    setNZ(A);
}
void CPU::eorZeroPage()
{
    uint8_t addr = bus.read(pc++);
    uint8_t value = bus.read(addr);

    A = A ^ value;
    setNZ(A);
}
void CPU::eorZeroPageX()
{
    uint8_t addr = bus.read(pc++);
    uint8_t value = bus.read((addr + X) & 0xFF);
    A = A ^ value;
    setNZ(A);
}
void CPU::eorAbsolute()
{
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);

    uint16_t finalAddr = (highByte << 8) | lowByte;

    uint8_t value = bus.read(finalAddr);
    A = A ^ value;
    setNZ(A);
}
void CPU::eorAbsoluteX()
{
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);

    uint16_t finalAddr = (highByte << 8) | lowByte;

    if ((finalAddr & 0xFF00) != ((finalAddr + X) & 0xFF00))
        cycles++;

    uint8_t value = bus.read(finalAddr + X);
    A = A ^ value;
    setNZ(A);
}
void CPU::eorAbsoluteY()
{
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);

    uint16_t finalAddr = (highByte << 8) | lowByte;

    if ((finalAddr & 0xFF00) != ((finalAddr + Y) & 0xFF00))
        cycles++;

    uint8_t value = bus.read(finalAddr + Y);
    A = A ^ value;
    setNZ(A);
}
void CPU::eorIndexedIndirect()
{
    uint8_t zp = bus.read(pc++);
    uint8_t ptr = bus.read(zp + X) & 0xFF;
    uint8_t lowByte = bus.read(ptr);
    uint8_t highByte = bus.read((ptr + 1) & 0xFF);
    uint16_t finalAddr = (highByte << 8) | lowByte;

    uint8_t value = bus.read(finalAddr);
    A = A ^ value;
    setNZ(A);
}
void CPU::eorIndirectIndexed()
{
    uint8_t zp = bus.read(pc++);
    uint8_t lowByte = bus.read(zp);
    uint8_t highByte = bus.read((zp + 1) & 0xFF);
    uint16_t finalAddr = (highByte << 8) | lowByte;

    if ((finalAddr & 0xFF00) != ((finalAddr + Y) & 0xFF00))
        cycles++;

    uint8_t value = bus.read(finalAddr + Y);
    A = A ^ value;
    setNZ(A);
}
void CPU::oraImmediate()
{
    uint8_t value = bus.read(pc++);
    A = A | value;
    setNZ(A);
}
void CPU::oraZeroPage()
{
    uint8_t addr = bus.read(pc++);
    uint8_t value = bus.read(addr);

    A = A | value;
    setNZ(A);
}
void CPU::oraZeroPageX()
{
    uint8_t addr = bus.read(pc++);
    uint8_t value = bus.read((addr + X) & 0xFF);
    A = A | value;
    setNZ(A);
}
void CPU::oraAbsolute()
{
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);

    uint16_t finalAddr = (highByte << 8) | lowByte;

    uint8_t value = bus.read(finalAddr);
    A = A | value;
    setNZ(A);
}
void CPU::oraAbsoluteX()
{
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);

    uint16_t finalAddr = (highByte << 8) | lowByte;

    if ((finalAddr & 0xFF00) != ((finalAddr + X) & 0xFF00))
        cycles++;

    uint8_t value = bus.read(finalAddr + X);
    A = A | value;
    setNZ(A);
}
void CPU::oraAbsoluteY()
{
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);

    uint16_t finalAddr = (highByte << 8) | lowByte;

    if ((finalAddr & 0xFF00) != ((finalAddr + Y) & 0xFF00))
        cycles++;

    uint8_t value = bus.read(finalAddr + Y);
    A = A | value;
    setNZ(A);
}
void CPU::oraIndexedIndirect()
{
    uint8_t zp = bus.read(pc++);
    uint8_t ptr = bus.read(zp + X) & 0xFF;
    uint8_t lowByte = bus.read(ptr);
    uint8_t highByte = bus.read((ptr + 1) & 0xFF);
    uint16_t finalAddr = (highByte << 8) | lowByte;

    uint8_t value = bus.read(finalAddr);
    A = A | value;
    setNZ(A);
}
void CPU::oraIndirectIndexed()
{
    uint8_t zp = bus.read(pc++);
    uint8_t lowByte = bus.read(zp);
    uint8_t highByte = bus.read((zp + 1) & 0xFF);
    uint16_t finalAddr = (highByte << 8) | lowByte;

    if ((finalAddr & 0xFF00) != ((finalAddr + Y) & 0xFF00))
        cycles++;

    uint8_t value = bus.read(finalAddr + Y);
    A = A | value;
    setNZ(A);
}
void CPU::cmpImmediate()
{
    compare(A, bus.read(pc++));
}
void CPU::cmpZeroPage()
{
    uint8_t addr = bus.read(pc++);
    compare(A, bus.read(addr));
}
void CPU::cmpZeroPageX()
{
    uint8_t addr = bus.read(pc++);
    compare(A, bus.read((addr + X) & 0xFF));
}
void CPU::cmpAbsolute()
{
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);
    uint16_t finalAddr = (highByte << 8) | lowByte;

    compare(A, bus.read(finalAddr));
}
void CPU::cmpAbsoluteX()
{
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);
    uint16_t finalAddr = (highByte << 8) | lowByte;

    if ((finalAddr & 0xFF00) != ((finalAddr + X) & 0xFF00))
        cycles++;

    compare(A, bus.read(finalAddr + X));
}
void CPU::cmpAbsoluteY()
{
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);
    uint16_t finalAddr = (highByte << 8) | lowByte;

    if ((finalAddr & 0xFF00) != ((finalAddr + Y) & 0xFF00))
        cycles++;

    compare(A, bus.read(finalAddr + Y));
}
void CPU::cmpIndexedIndirect()
{
    uint8_t zp = bus.read(pc++);
    uint8_t ptr = bus.read(zp + X) & 0xFF;
    uint8_t lowByte = bus.read(ptr);
    uint8_t highByte = bus.read((ptr + 1) & 0xFF);

    uint16_t finalAddr = (highByte << 8) | lowByte;

    compare(A, bus.read(finalAddr));
}
void CPU::cmpIndirectIndexed()
{
    uint8_t zp = bus.read(pc++);
    uint8_t lowByte = bus.read(zp);
    uint8_t highByte = bus.read((zp + 1) & 0xFF);

    uint16_t finalAddr = (highByte << 8) | lowByte;

    if ((finalAddr & 0xFF00) != ((finalAddr + Y) & 0xFF00))
        cycles++;

    compare(A, bus.read(finalAddr + Y));
}
void CPU::cpxImmediate()
{
    compare(X, bus.read(pc++));
}
void CPU::cpxZeroPage()
{
    uint8_t addr = bus.read(pc++);
    compare(X, bus.read(addr));
}
void CPU::cpxAbsolute()
{
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);
    uint16_t finalAddr = (highByte << 8) | lowByte;

    compare(X, bus.read(finalAddr));
}
void CPU::cpyImmediate()
{
    compare(Y, bus.read(pc++));
}
void CPU::cpyZeroPage()
{
    uint8_t addr = bus.read(pc++);
    compare(Y, bus.read(addr));
}
void CPU::cpyAbsolute()
{
    uint8_t lowByte = bus.read(pc++);
    uint8_t highByte = bus.read(pc++);
    uint16_t finalAddr = (highByte << 8) | lowByte;

    compare(Y, bus.read(finalAddr));
}
void CPU::aslAccumulator()
{
    if (A & 0x80)
    {
        P |= 0x01;
    }
    else
    {
        P &= ~0x01;
    }
    A = A <<= 1;
    setNZ(A);
}
void CPU::aslZeroPage()
{
    uint8_t addr = bus.read(pc++);
    uint8_t value = bus.read(addr);
    if (value & 0x80)
    {
        P |= 0x01;
    }
    else
    {
        P &= ~0x01;
    }
    value = value <<= 1;
    bus.write(addr, value);
    setNZ(value);
}
void CPU::aslZeroPageX()
{
    uint8_t addr = bus.read(pc++);
    uint8_t value = bus.read((addr + X) & 0xFF);
    if (value & 0x80)
    {
        P |= 0x01;
    }
    else
    {
        P &= ~0x01;
    }
    value = value <<= 1;
    bus.write((addr + X) & 0xFF, value);
    setNZ(value);
}
void CPU::aslAbsolute()
{
    uint8_t low = bus.read(pc++);
    uint8_t high = bus.read(pc++);

    uint16_t finalAddr = (high << 8) | low;
    uint8_t value = bus.read(finalAddr);
    if (value & 0x80)
    {
        P |= 0x01;
    }
    else
    {
        P &= ~0x01;
    }
    value = value <<= 1;
    bus.write(finalAddr, value);
    setNZ(value);
}
void CPU::aslAbsoluteX()
{
    uint8_t low = bus.read(pc++);
    uint8_t high = bus.read(pc++);

    uint16_t finalAddr = (high << 8) | low;
    uint8_t value = bus.read(finalAddr + X);
    if (value & 0x80)
    {
        P |= 0x01;
    }
    else
    {
        P &= ~0x01;
    }
    value = value <<= 1;
    bus.write(finalAddr + X, value);
    setNZ(value);
}
void CPU::lsrAccumulator()
{
    if (A & 0x01)
        P |= 0x01;
    else
        P &= ~0x01;

    A >>= 1;
    P &= ~0x80;

    if (A == 0)
        P |= 0x02;
    else
        P &= ~0x02;
}
void CPU::lsrZeroPage()
{
    uint8_t addr = bus.read(pc++);
    uint8_t value = bus.read(addr);

    if (value & 0x01)
        P |= 0x01;
    else
        P &= ~0x01;

    value >>= 1;
    P &= ~0x80;

    bus.write(addr, value);

    if (value == 0)
        P |= 0x02;
    else
        P &= ~0x02;
}
void CPU::lsrZeroPageX()
{
    uint8_t addr = bus.read(pc++);
    uint8_t effAddr = (addr + X) & 0xFF;
    uint8_t value = bus.read(effAddr);

    if (value & 0x01)
        P |= 0x01;
    else
        P &= ~0x01;

    value >>= 1;
    P &= ~0x80;

    bus.write(effAddr, value);

    if (value == 0)
        P |= 0x02;
    else
        P &= ~0x02;
}
void CPU::lsrAbsolute()
{
    uint8_t low = bus.read(pc++);
    uint8_t high = bus.read(pc++);
    uint16_t finalAddr = (high << 8) | low;
    uint8_t value = bus.read(finalAddr);

    if (value & 0x01)
        P |= 0x01;
    else
        P &= ~0x01;

    value >>= 1;
    P &= ~0x80;

    bus.write(finalAddr, value);

    if (value == 0)
        P |= 0x02;
    else
        P &= ~0x02;
}
void CPU::lsrAbsoluteX()
{
    uint8_t low = bus.read(pc++);
    uint8_t high = bus.read(pc++);
    uint16_t finalAddr = (high << 8) | low;
    uint16_t effAddr = finalAddr + X;
    uint8_t value = bus.read(effAddr);

    if (value & 0x01)
        P |= 0x01;
    else
        P &= ~0x01;

    value >>= 1;
    P &= ~0x80;

    bus.write(effAddr, value);

    if (value == 0)
        P |= 0x02;
    else
        P &= ~0x02;
}
void CPU::rorAccumulator()
{
    uint8_t oldCarry = P & 0x01;
    uint8_t newCarry = A & 0x01;

    A >>= 1;

    if (oldCarry)
        A |= 0x80;

    if (newCarry)
        P |= 0x01;
    else
        P &= ~0x01;
    setNZ(A);
}
void CPU::rorZeroPage()
{
    uint8_t zpAddr = bus.read(pc++);
    uint8_t value = bus.read(zpAddr);
    uint8_t oldC = P & 0x01;
    uint8_t newC = value & 0x01;

    value >>= 1;

    if (oldC)
        value |= 0x80;

    if (newC)
        P |= 0x01;
    else
        P &= ~0x01;
    bus.write(zpAddr, value);
    setNZ(value);
}
void CPU::rorZeroPageX()
{
    uint8_t zpAddr = bus.read(pc++);
    uint8_t value = bus.read((zpAddr + X) & 0xFF);
    uint8_t oldC = P & 0x01;
    uint8_t newC = value & 0x01;

    value >>= 1;

    if (oldC)
        value |= 0x80;

    if (newC)
        P |= 0x01;
    else
        P &= ~0x01;
    bus.write((zpAddr + X) & 0xFF, value);
    setNZ(value);
}
void CPU::rorAbsolute()
{
    uint8_t low = bus.read(pc++);
    uint8_t high = bus.read(pc++);

    uint16_t addr = (high << 8) | low;
    uint8_t value = bus.read(addr);

    uint8_t oldC = P & 0x01;
    uint8_t newC = value & 0x01;

    value >>= 1;

    if (oldC)
        value |= 0x80;

    if (newC)
        P |= 0x01;
    else
        P &= ~0x01;
    bus.write(addr, value);
    setNZ(value);
}
void CPU::rorAbsoluteX()
{
    uint8_t low = bus.read(pc++);
    uint8_t high = bus.read(pc++);

    uint16_t addr = (high << 8) | low;
    uint8_t value = bus.read(addr + X);

    uint8_t oldC = P & 0x01;
    uint8_t newC = value & 0x01;

    value >>= 1;

    if (oldC)
        value |= 0x80;

    if (newC)
        P |= 0x01;
    else
        P &= ~0x01;
    bus.write(addr + X, value);
    setNZ(value);
}
void CPU::rolAccumulator()
{
    uint8_t oldCarry = P & 0x01;
    uint8_t newCarry = (A & 0x80) >> 7;

    A <<= 1;

    if (oldCarry)
        A |= 0x01;

    if (newCarry)
        P |= 0x01;
    else
        P &= ~0x01;

    setNZ(A);
}

void CPU::rolZeroPage()
{
    uint8_t zpAddr = bus.read(pc++);
    uint8_t value = bus.read(zpAddr);

    uint8_t oldCarry = P & 0x01;
    uint8_t newCarry = (value & 0x80) >> 7;

    value <<= 1;

    if (oldCarry)
        value |= 0x01;

    if (newCarry)
        P |= 0x01;
    else
        P &= ~0x01;

    bus.write(zpAddr, value);
    setNZ(value);
}

void CPU::rolZeroPageX()
{
    uint8_t zpAddr = bus.read(pc++);
    uint8_t addr = (zpAddr + X) & 0xFF;
    uint8_t value = bus.read(addr);

    uint8_t oldCarry = P & 0x01;
    uint8_t newCarry = (value & 0x80) >> 7;

    value <<= 1;

    if (oldCarry)
        value |= 0x01;

    if (newCarry)
        P |= 0x01;
    else
        P &= ~0x01;

    bus.write(addr, value);
    setNZ(value);
}

void CPU::rolAbsolute()
{
    uint8_t low = bus.read(pc++);
    uint8_t high = bus.read(pc++);
    uint16_t addr = (high << 8) | low;
    uint8_t value = bus.read(addr);

    uint8_t oldCarry = P & 0x01;
    uint8_t newCarry = (value & 0x80) >> 7;

    value <<= 1;

    if (oldCarry)
        value |= 0x01;

    if (newCarry)
        P |= 0x01;
    else
        P &= ~0x01;

    bus.write(addr, value);
    setNZ(value);
}

void CPU::rolAbsoluteX()
{
    uint8_t low = bus.read(pc++);
    uint8_t high = bus.read(pc++);
    uint16_t addr = ((high << 8) | low) + X;
    uint8_t value = bus.read(addr);

    uint8_t oldCarry = P & 0x01;
    uint8_t newCarry = (value & 0x80) >> 7;

    value <<= 1;

    if (oldCarry)
        value |= 0x01;

    if (newCarry)
        P |= 0x01;
    else
        P &= ~0x01;

    bus.write(addr, value);
    setNZ(value);
}

void CPU::clock()
{
    if (cycles == 0)
    {
        uint8_t opcode = bus.read(pc++);
        Instruction &instr = table[opcode];

        if (!instr.execute)
        {
            std::cerr << "CRASH: opcode " << std::hex << opcode << " has null execute pointer!\n";
            return;
        }

        (this->*instr.execute)();
        cycles = instr.cycles;

        std::cout << "IRQ timing: " << ((P & 0x04) != 0) << " Opcode: " << std::hex << static_cast<int>(opcode) << std::endl;
    }
    cycles--;

    if (NMI)
    {
        cycles = 0;
        NMI = false;
        pushPC();
        pushStatus();
        pc = bus.read(0xFFFA);
        cycles += 7;
    }

    if (IRQ && !(P & 0x04))
    {
        IRQ = false;

        pushPC();
        pushStatus(false);

        P |= 0x04;

        uint8_t low = bus.read(0xFFFE);
        uint8_t high = bus.read(0xFFFF);

        pc = (high << 8) | low;

        cycles = 7;
    }
}
void CPU::connectBus(Bus *b)
{
    bus = *b;
}
