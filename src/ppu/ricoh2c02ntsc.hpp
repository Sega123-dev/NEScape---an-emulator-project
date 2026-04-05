#pragma once
#include <cstdint>
#include <iostream>
#include <vector>
#include "../mappers/mapper.hpp"
#include "../bus/bus.hpp"
#include <SDL2/SDL.h>
class Bus;
class PPU
{
public:
    PPU(std::vector<uint8_t> &chrROMBuffer);
    PPU() = default;
    uint8_t vram[2048];
    uint8_t paletteRAM[32];
    uint8_t oam[256];

    uint8_t secondaryOAM[8 * 4];

    static constexpr const uint32_t PPU_CLOCK_SPEED = 5369317;

    uint8_t PPUCTRL;
    uint8_t PPUMASK;
    uint8_t PPUSTATUS;
    uint8_t OAMADDR;

    std::vector<uint8_t> chrROM;

    uint16_t v; // Current VRAM addr
    uint16_t t; // Temp VRAM addr
    uint8_t x;  // Fine X scroll
    bool w;     // Write toggle

    void clock();

    void write2000(uint8_t data);
    void write2001(uint8_t data);
    uint8_t read2002();
    void write2005(uint8_t data);
    void write2006(uint8_t data);
    uint8_t read2007();
    void write2007(uint8_t data);

    int scanline;
    int cycle;

    bool vblankFlag;
    bool sprite0Hit;
    bool spriteOverflow;
    uint8_t ppuOpenBus;

    // For Background

    uint8_t bgPixel;
    uint8_t bgPalette;
    uint8_t colorIndex;

    // Sprite fetch

    uint8_t spriteCount;
    uint8_t spritePatternLow[8];
    uint8_t spritePatternHigh[8];
    uint8_t spriteAttributes[8];
    uint8_t spriteRowBuffer[8];
    uint8_t spriteX[8];

    // Sprite render

    uint8_t spritePalette[8];
    uint8_t spritePriority[8];
    uint8_t spriteXCounter[8];
    uint8_t spriteShiftLow[8];
    uint8_t spriteShiftHigh[8];

    bool spriteZeroPossible = false;
    bool spriteZeroRendering;
    bool showSpritesLeft;

    uint8_t spritePixel;

    // Raw reads/writes

    uint8_t ppuReadRaw(uint16_t addr);
    void ppuWriteRaw(uint16_t addr, uint8_t data);

    // Mapper instance

    Mapper *mapper;
    bool horizontalMirroring;

    // Write $2000

    int vramIncrement;
    uint16_t bgPatternTable;
    uint16_t spritePatternTable;
    int spriteHeight;
    bool nmiOutput;

    // Write $2001

    bool showBackground;
    bool showSprites;
    bool showLeftBackground;
    bool showLeftSprites;
    bool grayscaleMode;

    bool emphasizeRed;
    bool emphasizeGreen;
    bool emphasizeBlue;

    // Read $2007
    uint16_t ppuDataBuffer;
    struct Color
    {
        uint8_t r, g, b;
    };
    Color framebuffer[240][256]; // Screen buffer

    uint16_t bgShiftLow = 0;
    uint16_t bgShiftHigh = 0;
    uint16_t attrShiftLow = 0;
    uint16_t attrShiftHigh = 0;

    uint8_t nextTileID = 0;
    uint8_t nextTileAttr = 0;
    uint8_t nextTileLow = 0;
    uint8_t nextTileHigh = 0;

    // NTSC

    bool isOddFrame = true;

    // Background shift register helpers
    void loadBackgroundShifters();

    // Scrolling helpers
    void incrementX();
    void incrementY();
    void copyHorizontal();
    void copyVertical();

    // Used to store an address for A12 in MMC3

    uint16_t getLastAddress();
    uint16_t lastPPUAddress;

    void connectBus(Bus *b);
    void drawFrame();
    void initWindow();

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *texture = nullptr;
};