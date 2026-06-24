#pragma once
#include <iostream>
#include <cstdint>
#include <vector>
#include <memory>

enum Mirror
{
    HORIZONTAL,
    VERTICAL,
    ONESCREENLOW,
    ONESCREENHIGH
};

class Mapper
{
public:
    virtual uint8_t cpuRead(uint16_t addr) = 0;
    virtual uint8_t ppuRead(uint16_t addr) = 0;
    virtual void cpuWrite(uint16_t addr, uint8_t data) = 0;
    virtual void ppuWrite(uint16_t addr, uint8_t data) = 0;

    virtual Mirror getMirror() = 0;
    virtual void reset() = 0;

    ~Mapper() = default;
};