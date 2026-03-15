#pragma once
#include <cstdint>
#include <iostream>
#include "../ppu/ricoh2c02ntsc.hpp"
enum AddressingMode
{
    IMPLIED,
    ACCUMULATOR,
    IMMEDIATE,
    ZERO_PAGE,
    ZERO_PAGE_X,
    ZERO_PAGE_Y,
    ABSOLUTE,
    RELATIVE,
    ABSOLUTE_X,
    ABSOLUTE_Y,
    INDIRECT,
    INDEXED_INDIRECT,
    INDIRECT_INDEXED
};

class CPU;
class Bus;
class PPU;
struct Instruction
{
    std::string name;
    void (CPU::*execute)();
    AddressingMode mode;
    int cycles;
};

extern Instruction table[256];

class CPU
{

public:
    Bus *bus = nullptr;
    Instruction table[256];
    CPU();
    uint8_t dataBus, A, X, Y, P, sp, cycles, opState;
    uint16_t pc;
    bool NMI, IRQ, RESET;
    const uint32_t CPU_CLOCK_SPEED = PPU::PPU_CLOCK_SPEED / 3;

    void reset();
    void clock();
    void connectBus(Bus *b);

    void brk();
    void nop();
    void rti();

    void bitAbsolute();
    void bitZeroPage();

    void incAbsolute();
    void incAbsoluteX();
    void incZeroPage();
    void incZeroPageX();

    void decAbsolute();
    void decAbsoluteX();
    void decZeroPage();
    void decZeroPageX();

    void inx();
    void iny();

    void dex();
    void dey();

    void clc();
    void sec();

    void sei();
    void cli();

    void clv();

    void ldaImmediate();
    void ldaZeroPage();
    void ldaZeroPageX();
    void ldaAbsolute();
    void ldaAbsoluteX();
    void ldaAbsoluteY();
    void ldaIndexedIndirect();
    void ldaIndirectIndexed();

    void ldxImmediate();
    void ldxZeroPage();
    void ldxZeroPageY();
    void ldxAbsolute();
    void ldxAbsoluteY();

    void ldyImmediate();
    void ldyZeroPage();
    void ldyZeroPageX();
    void ldyAbsolute();
    void ldyAbsoluteX();

    void staZeroPage();
    void staZeroPageX();
    void staAbsolute();
    void staAbsoluteX();
    void staAbsoluteY();
    void staIndexedIndirect();
    void staIndirectIndexed();

    void stxZeroPage();
    void stxZeroPageY();
    void stxAbsolute();

    void styZeroPage();
    void styZeroPageX();
    void styAbsolute();

    void tax();
    void tay();
    void txa();
    void tya();

    void tsx();
    void txs();

    void pha();
    void pla();
    void php();
    void plp();

    void jmpAbsolute();
    void jmpIndirect();

    void jsrAbsolute();
    void rts();

    void adcImmediate();
    void adcZeroPage();
    void adcZeroPageX();
    void adcAbsolute();
    void adcAbsoluteX();
    void adcAbsoluteY();
    void adcIndexedIndirect();
    void adcIndirectIndexed();

    void sbcImmediate();
    void sbcZeroPage();
    void sbcZeroPageX();
    void sbcAbsolute();
    void sbcAbsoluteX();
    void sbcAbsoluteY();
    void sbcIndexedIndirect();
    void sbcIndirectIndexed();

    void bcc();
    void bcs();
    void beq();
    void bne();
    void bmi();
    void bpl();
    void bvc();
    void bvs();

    void andImmediate();
    void andZeroPage();
    void andZeroPageX();
    void andAbsolute();
    void andAbsoluteX();
    void andAbsoluteY();
    void andIndexedIndirect();
    void andIndirectIndexed();

    void eorImmediate();
    void eorZeroPage();
    void eorZeroPageX();
    void eorAbsolute();
    void eorAbsoluteX();
    void eorAbsoluteY();
    void eorIndexedIndirect();
    void eorIndirectIndexed();

    void oraImmediate();
    void oraZeroPage();
    void oraZeroPageX();
    void oraAbsolute();
    void oraAbsoluteX();
    void oraAbsoluteY();
    void oraIndexedIndirect();
    void oraIndirectIndexed();

    void cmpImmediate();
    void cmpZeroPage();
    void cmpZeroPageX();
    void cmpAbsolute();
    void cmpAbsoluteX();
    void cmpAbsoluteY();
    void cmpIndexedIndirect();
    void cmpIndirectIndexed();

    void cpxImmediate();
    void cpxZeroPage();
    void cpxAbsolute();

    void cpyImmediate();
    void cpyZeroPage();
    void cpyAbsolute();

    void aslAccumulator();
    void aslZeroPage();
    void aslZeroPageX();
    void aslAbsolute();
    void aslAbsoluteX();

    void lsrAccumulator();
    void lsrZeroPage();
    void lsrZeroPageX();
    void lsrAbsolute();
    void lsrAbsoluteX();

    void rorAccumulator();
    void rorZeroPage();
    void rorZeroPageX();
    void rorAbsolute();
    void rorAbsoluteX();

    void rolAccumulator();
    void rolZeroPage();
    void rolZeroPageX();
    void rolAbsolute();
    void rolAbsoluteX();

    void setNZ(uint8_t reg);
    void setVC(uint8_t V, uint8_t C);
    void compare(uint8_t reg, uint8_t value);
    void setIRQ();
    void clearIRQ();
    void pushPC();
    void pushStatus(bool breakFlag = false);
};