#include <cstdint>
#include <iostream>
#include <vector>
#include <SDL.h>
#include "ricoh2c02ntsc.hpp"
#include "../mappers/iNES_reader/reader.hpp"
#include "../mappers/mapper.hpp"
#include "../cpu/6502ricoh.hpp"
#undef main // Important for MinGW/SDL2 on Windows

// FILES NOT COMPILED,SDL WILL NOT WORK UNTIL YOU COMPILE!!!
Mapper *mapper;
Bus *bus = nullptr;
CPU *cpu;

PPU::PPU(std::vector<uint8_t> &chrROMBuffer)
{
    chrROM = chrROMBuffer;

    for (int i = 0; i < 2048; i++)
        vram[i] = 0;
    for (int i = 0; i < 32; i++)
        paletteRAM[i] = 0;
    for (int i = 0; i < 256; i++)
        oam[i] = 0;

    PPUCTRL = 0;
    PPUMASK = 0;
    PPUSTATUS = 0xA0;
    OAMADDR = 0;

    v = 0;
    t = 0;
    x = 0;
    w = false;

    scanline = -1;
    cycle = 0;

    Mirror mirroring = mapper->getMirror();
}

// HELPERS

PPU::Color nesPalette[64] =
    {
        {84, 84, 84}, {0, 30, 116}, {8, 16, 144}, {48, 0, 136}, {68, 0, 100}, {92, 0, 48}, {84, 4, 0}, {60, 24, 0}, {32, 42, 0}, {8, 58, 0}, {0, 64, 0}, {0, 60, 0}, {0, 50, 60}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},

        {152, 150, 152},
        {8, 76, 196},
        {48, 50, 236},
        {92, 30, 228},
        {136, 20, 176},
        {160, 20, 100},
        {152, 34, 32},
        {120, 60, 0},
        {84, 90, 0},
        {40, 114, 0},
        {8, 124, 0},
        {0, 118, 40},
        {0, 102, 120},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},

        {236, 238, 236},
        {76, 154, 236},
        {120, 124, 236},
        {176, 98, 236},
        {228, 84, 236},
        {236, 88, 180},
        {236, 106, 100},
        {212, 136, 32},
        {160, 170, 0},
        {116, 196, 0},
        {76, 208, 32},
        {56, 204, 108},
        {56, 180, 204},
        {60, 60, 60},
        {0, 0, 0},
        {0, 0, 0},

        {236, 238, 236},
        {168, 204, 236},
        {188, 188, 236},
        {212, 178, 236},
        {236, 174, 236},
        {236, 174, 212},
        {236, 180, 176},
        {228, 196, 144},
        {204, 210, 120},
        {180, 222, 120},
        {168, 226, 144},
        {152, 226, 180},
        {160, 214, 228},
        {160, 162, 160},
        {0, 0, 0},
        {0, 0, 0}};

void PPU::loadBackgroundShifters()
{
    bgShiftLow = (bgShiftLow & 0xFF00) | nextTileLow;
    bgShiftHigh = (bgShiftHigh & 0xFF00) | nextTileHigh;

    uint8_t palette =
        (nextTileAttr >>
         (((v >> 4) & 4) | (v & 2))) &
        0x03;

    attrShiftLow = (attrShiftLow & 0xFF00) | ((palette & 1) ? 0xFF : 0x00);
    attrShiftHigh = (attrShiftHigh & 0xFF00) | ((palette & 2) ? 0xFF : 0x00);
}

void PPU::incrementX()
{
    if ((v & 0x001F) == 31)
    {
        v &= ~0x001F;
        v ^= 0x0400;
    }
    else
        v++;
}

void PPU::incrementY()
{
    if ((v & 0x7000) != 0x7000)
        v += 0x1000;
    else
    {
        v &= ~0x7000;
        uint16_t y = (v & 0x03E0) >> 5;
        if (y == 29)
        {
            y = 0;
            v ^= 0x0800;
        }
        else if (y == 31)
            y = 0;
        else
            y++;
        v = (v & ~0x03E0) | (y << 5);
    }
}

void PPU::copyHorizontal()
{
    v = (v & ~0x041F) | (t & 0x041F);
}

void PPU::copyVertical()
{
    v = (v & ~0x7BE0) | (t & 0x7BE0);
}

uint16_t PPU::getLastAddress()
{
    return lastPPUAddress;
}

uint8_t PPU ::ppuReadRaw(uint16_t addr)
{
    addr = addr & 0x3FFF;

    if (addr >= 0x3000 && addr <= 0x3EFF)
        addr -= 0x1000;
    lastPPUAddress = addr;
    unsigned int VRAMindex;

    if (addr >= 0x0000 && addr <= 0x1FFF)
        return mapper->ppuRead(addr);
    lastPPUAddress = addr;
    if (addr >= 0x2000 && addr <= 0x2FFF)
    {
        uint8_t NTindex = (addr - 0x2000) / 0x400;
        unsigned int offset = (addr - 0x2000) % 0x400;

        if (horizontalMirroring)
        {
            VRAMindex = (NTindex / 2) * 0x400 + offset;
        }
        else
        {
            VRAMindex = (NTindex % 2) * 0x400 + offset;
        }
        lastPPUAddress = addr;
        return vram[VRAMindex];
    }

    if (addr >= 0x3F00 && addr <= 0x3FFF)
    {
        addr = 0x3F00 + (addr % 0x20);

        if (addr == 0x3F10)
            addr = 0x3F00;
        if (addr == 0x3F14)
            addr = 0x3F04;
        if (addr == 0x3F18)
            addr = 0x3F08;
        if (addr == 0x3F1C)
            addr = 0x3F0C;

        lastPPUAddress = addr;

        return paletteRAM[addr - 0x3F00];
    }
    return 0;
}
void PPU::ppuWriteRaw(uint16_t addr, uint8_t data)
{
    addr = addr & 0x3FFF;

    if (addr >= 0x3000 && addr <= 0x3EFF)
        addr -= 0x1000;
    lastPPUAddress = addr;
    unsigned int VRAMindex;

    if (addr >= 0x0000 && addr <= 0x1FFF)
        mapper->ppuWrite(addr, data);
    lastPPUAddress = addr;
    if (addr >= 0x2000 && addr <= 0x2FFF)
    {
        uint8_t NTindex = (addr - 0x2000) / 0x400;
        unsigned int offset = (addr - 0x2000) % 0x400;

        if (horizontalMirroring)
        {
            VRAMindex = (NTindex / 2) * 0x400 + offset;
        }
        else
        {
            VRAMindex = (NTindex % 2) * 0x400 + offset;
        }

        vram[VRAMindex] = data;
        lastPPUAddress = addr;
    }

    if (addr >= 0x3F00 && addr <= 0x3FFF)
    {
        addr = 0x3F00 + (addr % 0x20);

        if (addr == 0x3F10)
            addr = 0x3F00;
        if (addr == 0x3F14)
            addr = 0x3F04;
        if (addr == 0x3F18)
            addr = 0x3F08;
        if (addr == 0x3F1C)
            addr = 0x3F0C;

        paletteRAM[addr - 0x3F00] = data;
        lastPPUAddress = addr;
    }
    ppuOpenBus = data;
}

void PPU::write2000(uint8_t data)
{
    PPUCTRL = data;

    t = (t & 0x73FF) | ((data & 0x03) << 10);

    vramIncrement = (data & 0x04) ? 32 : 1;

    bgPatternTable = (data & 0x10) ? 0x1000 : 0x0000;

    spritePatternTable = (data & 0x08) ? 0x1000 : 0x0000;

    spriteHeight = (data & 0x20) ? 16 : 8;

    nmiOutput = (data & 0x80) != 0; // Update the NMI in the CPU when VBLANK is set
}
void PPU::write2001(uint8_t data)
{
    PPUMASK = data;

    grayscaleMode = data & 0x01; // Set it during the read/write

    showLeftBackground = data & 0x02;
    showLeftSprites = data & 0x04;
    showBackground = data & 0x08;
    showSprites = data & 0x10;

    emphasizeRed = data & 0x20;
    emphasizeGreen = data & 0x40;
    emphasizeBlue = data & 0x80;
}
uint8_t PPU::read2002()
{
    PPUSTATUS = (vblankFlag << 7) | (sprite0Hit << 6) | (spriteOverflow << 5) | (ppuOpenBus & 0x1F);
    vblankFlag = false;
    w = false;
    return PPUSTATUS;
}
void PPU::write2006(uint8_t data)
{
    if (w == 0)
    {
        t = (t & 0x00FF) | ((data & 0x3F) << 8);
        w = 1;
    }
    else
    {
        t = (t & 0xFF00) | data;
        v = t;
        w = 0;
    }
}
void PPU::write2005(uint8_t data)
{
    if (w == 0)
    {
        x = data & 0x07;
        t = (t & 0xFFE0) | ((data >> 3) & 0x1F);
        w = 1;
    }
    else
    {
        t = (t & 0x8FFF) |
            ((data & 0x07) << 12);

        t = (t & 0xFC1F) |
            ((data >> 3) & 0x1F) << 5;
        w = 0;
    }
}
void PPU::write2007(uint8_t data)
{
    uint16_t addr = v & 0x3FFF;
    if (addr < 0x2000)
    {
        mapper->ppuWrite(addr, data);
        lastPPUAddress = addr;
    }
    else if (addr < 0x3F00)
    {
        mapper->ppuWrite(addr, data);
        lastPPUAddress = addr;
    }
    else
    {
        uint8_t palAddr = (addr - 0x3F00) & 0x1F;
        if (palAddr == 0x10)
            palAddr = 0x00;
        if (palAddr == 0x14)
            palAddr = 0x04;
        if (palAddr == 0x18)
            palAddr = 0x08;
        if (palAddr == 0x1C)
            palAddr = 0x0C;
        paletteRAM[palAddr] = data;
        lastPPUAddress = addr;
    }
    v += (PPUCTRL & 0x04) ? 32 : 1;
    lastPPUAddress = addr;
}
uint8_t PPU::read2007()
{
    uint16_t addr = v & 0x3FFF;
    uint8_t returned; // For keeping the old value
    if (addr < 0x3F00)
    {
        returned = ppuDataBuffer;
        ppuDataBuffer = mapper->ppuRead(addr);
    }

    else if (addr >= 0x3F00)
    {
        uint8_t palAddr = (addr - 0x3F00) & 0x1F;
        if (palAddr == 0x10)
            palAddr = 0x00;
        if (palAddr == 0x14)
            palAddr = 0x04;
        if (palAddr == 0x18)
            palAddr = 0x08;
        if (palAddr == 0x1C)
            palAddr = 0x0C;

        returned = paletteRAM[palAddr];

        ppuDataBuffer = mapper->ppuRead(addr - 0x1000);
    }
    v += (PPUCTRL & 0x04) ? 32 : 1;
    lastPPUAddress = addr;
    return returned;
}
void PPU::clock()
{
    spriteHeight = (PPUCTRL & 0x20) ? 16 : 8; // Needs to be set before any first visible scanlines

    bool renderingEnabled = showBackground || showSprites;

    // ODD FRAMES NTSC SYNC

    if (scanline == -1)
    {
        if (cycle == 340 && isOddFrame && showBackground)
        {
            cycle++;
        }
    }

    if (scanline == 261 && cycle == 340)
        isOddFrame = !isOddFrame;

    if (scanline >= -1 && scanline < 240)
    {

        // PRE-RENDER SETS

        if (scanline == -1 && cycle == 1)
        {
            vblankFlag = false;
            sprite0Hit = false;
            spriteOverflow = false;
        }
        if (renderingEnabled &&
            cycle >= 1 && cycle <= 256)
        {
            bgShiftLow <<= 1;
            bgShiftHigh <<= 1;
            attrShiftLow <<= 1;
            attrShiftHigh <<= 1;
        }

        // BACKGROUND SHIFTING

        if (renderingEnabled &&
            ((cycle >= 1 && cycle <= 256) || (cycle >= 321 && cycle <= 336)))
        {
            switch (cycle % 8)
            {
            case 1:
                nextTileID = ppuReadRaw(0x2000 | (v & 0x0FFF));
                break;

            case 3:
                nextTileAttr = ppuReadRaw(
                    0x23C0 |
                    (v & 0x0C00) |
                    ((v >> 4) & 0x38) |
                    ((v >> 2) & 0x07));
                break;

            case 5:
                nextTileLow = ppuReadRaw(
                    bgPatternTable +
                    nextTileID * 16 +
                    ((v >> 12) & 7));
                break;

            case 7:
                nextTileHigh = ppuReadRaw(
                    bgPatternTable +
                    nextTileID * 16 +
                    ((v >> 12) & 7) + 8);
                break;

            case 0:
                loadBackgroundShifters();
                incrementX();
                break;
            }
        }
        if (renderingEnabled)
        {
            if (cycle == 256)
                incrementY();

            if (cycle == 257)
                copyHorizontal();

            if (scanline == -1 && cycle >= 280 && cycle <= 304)
                copyVertical();
        }
    }
    if (scanline >= 0 && scanline < 240 &&
        cycle >= 1 && cycle <= 256)
    {
        uint8_t bit = 15 - x;

        bgPixel =
            ((bgShiftHigh >> bit) & 1) << 1 |
            ((bgShiftLow >> bit) & 1);

        bgPalette =
            ((attrShiftHigh >> bit) & 1) << 1 |
            ((attrShiftLow >> bit) & 1);

        colorIndex =
            bgPixel ? ((bgPalette << 2) | bgPixel) : 0;
    }

    // SPRITE SHIFTING

    if (scanline >= 0 && scanline < 240)
    {
        if (cycle >= 1 && cycle <= 256)
        {
            for (int i = 0; i < spriteCount; i++)
            {
                if (spriteXCounter[i] > 0)
                {
                    spriteXCounter[i]--;
                }
                else
                {
                    spriteShiftLow[i] <<= 1;
                    spriteShiftHigh[i] <<= 1;
                }
            }
        }
    }

    // OAM PATTERN SCAN

    if (cycle == 65)
    {
        memset(secondaryOAM, 0, 32);
        spriteCount = 0;
    }
    if (cycle >= 65 && cycle <= 256)
    {
        for (int i = 0; i < 64; i++)
        {
            uint8_t Ypos = oam[i * 4 + 0];

            if (scanline >= Ypos && scanline < Ypos + spriteHeight)
            {
                spriteAttributes[i] = secondaryOAM[i * 4 + 2];
                spriteX[i] = secondaryOAM[i * 4 + 3];
                if (spriteCount >= 8)
                {
                    spriteOverflow = true;
                    break;
                }
                for (int j = 0; j < 4; j++)
                {
                    secondaryOAM[spriteCount * 4 + j] = oam[i * 4 + j];
                }
                spriteCount++;
            }
        }
    }

    // SET VBLANK

    if (scanline == 241 && cycle == 1)
    {
        vblankFlag = true;
        if (vblankFlag && nmiOutput)
            cpu->NMI = true;
    }

    // SPRITE EVALUATION

    if (cycle >= 256 && cycle <= 320)
    {
        for (int i = 0; i < spriteCount; i++)
        {
            uint8_t spriteY = secondaryOAM[i * 4 + 0];
            uint8_t Xsprite = secondaryOAM[i * 4 + 3];
            uint8_t spriteRow = scanline - spriteY;
            uint16_t patternTableBase;
            uint8_t tileIndex = secondaryOAM[i * 4 + 1];
            uint16_t patternTableAddr;

            if (secondaryOAM[i * 4 + 2] & 0x80)
                spriteRow = (spriteHeight - 1) - spriteRow;

            spriteRowBuffer[i] = spriteRow;

            if (spriteHeight == 8)
            {
                patternTableBase = (PPUCTRL & 0x08) ? 0x1000 : 0x0000;
                patternTableAddr = patternTableBase + tileIndex * 16 + spriteRow;
            }
            else
            {
                patternTableBase = (tileIndex & 1) ? 0x1000 : 0x0000;

                if (spriteRow < 8)
                    patternTableAddr = patternTableBase + (tileIndex & 0xFE) * 16 + spriteRow;
                else
                    patternTableAddr = patternTableBase + (tileIndex | 0x01) * 16 + (spriteRow - 8);
            }

            spritePatternLow[i] = ppuReadRaw(patternTableAddr);
            spritePatternHigh[i] = ppuReadRaw(patternTableAddr + 8);
        }
    }
    // SPRITE RENDERING

    if (showSprites && cycle >= 1 && cycle <= 256)
    {
        spritePixel = 0;
        uint8_t spritePaletteIndex = 0;
        bool spriteForeground = false;

        for (int i = 0; i < spriteCount; i++)
        {
            spriteShiftLow[i] = spritePatternLow[i];
            spriteShiftHigh[i] = spritePatternHigh[i];
            spriteXCounter[i] = secondaryOAM[i * 4 + 3];
            spritePalette[i] = secondaryOAM[i * 4 + 2] & 0x03;
            spritePriority[i] = secondaryOAM[i * 4 + 2] & 0x20;

            if (spriteXCounter[i] == 0)
            {
                uint8_t pixel = ((spriteShiftHigh[i] & 0x80) >> 6) |
                                ((spriteShiftLow[i] & 0x80) >> 7);

                if (pixel != 0)
                {
                    spritePixel = pixel;
                    spritePaletteIndex = spritePalette[i];
                    spriteForeground = !spritePriority[i];

                    if (i == 0)
                        spriteZeroRendering = true;
                    break;
                }
            }
        }
        if (cycle < 9 && !showSpritesLeft)
            spritePixel = 0;

        for (int i = 0; i < spriteCount; i++)
        {
            if (spriteXCounter[i] > 0)
                spriteXCounter[i]--;
            else
            {
                spriteShiftLow[i] <<= 1;
                spriteShiftHigh[i] <<= 1;
            }
        }

        // PRIORITY

        if (bgPixel == 0 && spritePixel == 0)
            colorIndex = 0;
        else if (bgPixel == 0 && spritePixel > 0)
            colorIndex = 0x10 | (spritePaletteIndex << 2) | spritePixel;
        else if (bgPixel > 0 && spritePixel == 0)
            colorIndex = (bgPalette << 2) | bgPixel;
        else
        {
            if (spriteForeground)
                colorIndex = 0x10 | (spritePaletteIndex << 2) | spritePixel;
            else
                colorIndex = (bgPalette << 2) | bgPixel;
        }

        // PIXEL EXTRACTING

        uint8_t paletteLookup = paletteRAM[colorIndex & 0x1F];

        Color c = nesPalette[paletteLookup & 0x3F];

        framebuffer[scanline][cycle - 1] = c;

        // SPRITE HORIZONTAL FLIP

        uint8_t flipped;
        spritePixel = 0;
        for (int i = 0; i < spriteCount; i++)
        {

            bool hFlip = spriteAttributes[i] & 0x40;
            bool vFlip = spriteAttributes[i] & 0x80;

            if (hFlip)
                flipped = 7 - spriteXCounter[i];
            else
                flipped = spriteXCounter[i];

            uint8_t spriteY = secondaryOAM[i * 4 + 0];
            uint8_t spriteRow = scanline - spriteY;
            uint8_t tileIndex = secondaryOAM[i * 4 + 1];
            uint16_t patternTableAddr;
            uint16_t patternTableBase = (PPUCTRL & 0x08) ? 0x1000 : 0x0000;

            if (vFlip)
                spriteRow = spriteHeight - 1 - spriteRow;

            if (spriteHeight == 8)
            {
                patternTableBase = (PPUCTRL & 0x08) ? 0x1000 : 0x0000;
                patternTableAddr = patternTableBase + tileIndex * 16 + spriteRow;
            }
            else
            {
                patternTableBase = (tileIndex & 1) ? 0x1000 : 0x0000;

                if (spriteRow < 8)
                    patternTableAddr = patternTableBase + (tileIndex & 0xFE) * 16 + spriteRow;
                else
                    patternTableAddr = patternTableBase + (tileIndex | 0x01) * 16 + (spriteRow - 8);
            }

            spritePatternLow[i] = ppuReadRaw(patternTableAddr + spriteRow);
            spritePatternHigh[i] = ppuReadRaw(patternTableAddr + spriteRow + 8);

            uint8_t pixel = ((spriteShiftHigh[i] >> flipped) & 1) << 1 |
                            ((spriteShiftLow[i] >> flipped) & 1);
            if (pixel != 0)
            {
                spritePixel = pixel;
                break;
            }
        }

        // LEFT EDGE BG

        if (cycle <= 8 && !showLeftBackground && !showLeftSprites)
            bgPixel = 0;
        if (cycle <= 8 && !showLeftBackground && !showLeftSprites)
            spritePixel = 0;

        if (spriteZeroRendering && spriteZeroPossible && bgPixel != 0 && spritePixel != 0 && cycle >= 1 && cycle <= 255 && !(cycle <= 8 && (!showLeftBackground || !showLeftSprites)))
        {
            sprite0Hit = true; // Sprite 0 hit
        }
    }

    // SCREEN

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *texture = nullptr;

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
        "NEScape",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        256 * 3,
        240 * 3,
        0);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGB24,
        SDL_TEXTUREACCESS_STREAMING,
        256,
        240);
    // CYCLE INCREMENT

    cycle++;
    if (cycle > 340)
    {
        cycle = 0;
        scanline++;
        if (scanline > 261)
            scanline = -1;
    };
}
void PPU::drawFrame()
{

    // texture conversion

    uint8_t pixels[256 * 240 * 3];

    for (int y = 0; y < 240; y++)
    {
        for (int x = 0; x < 256; x++)
        {
            Color c = framebuffer[y][x];

            int i = (y * 256 + x) * 3;

            pixels[i] = c.r;
            pixels[i + 1] = c.g;
            pixels[i + 2] = c.b;
        }
    };

    // passing the texture to GPU

    SDL_UpdateTexture(texture, NULL, pixels, 256 * 3);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}
void PPU::connectBus(Bus *b)
{
    bus = b;
}
////////////////////////////////////////////
// CLOCK FUNCTION AROUND 75% ACCURACY GOAL//
////////////////////////////////////////////