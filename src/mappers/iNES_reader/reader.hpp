#pragma once
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

class Mapper;
class Bus;
void dumpROM(const char *filename);
Mapper *chooseMapper();

extern uint8_t header[16];
extern int prgRomSize;
extern int chrRomSize;
extern uint8_t flag6;
extern uint8_t flag7;
extern int mapperID;
extern bool verticalMirroring;
extern bool SRAM;
extern bool fourScreenMirroring;
extern std::vector<uint8_t> PRG_ROM;
extern std::vector<uint8_t> CHR_ROM;

void connectReader(Bus *b);