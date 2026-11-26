# NEScape

# What is an emulator?

Emulator is a software that mimics the real hardware.This repository is example of an emulator based on a legacy 8-bit console manufactured in 1983 called **Nintendo Entertainment System(NES)**,also known as **Famicom**, in which development,various companies worked,mainly Nintendo.Like other emulators,this emulator won't be 100% accurate as a real console.In theory, the goal accuracy of this emulator is around 65% if we do not include the pirated and homebrew aspects of this console.With this emulator in theory,you should be able to play around 70% of commercial titles when I release the full emulator demo.
The best comparison to this emulator could be the earliest versions of FCEUX or Nestopia emulators.

# What are the components of the NES?

**Ricoh A6502 CPU** - the variation of MOS 6502 CPU processor where the decimal(D) flag is ignored.The CPU has 151 official opcodes,3 main registers(Accumulator `A`,`X`,`Y`),program counter,stack,addressable program memory etc.

**Address Bus** - plate where all components are wired.It is used for comumnication between all components of the NES using hexadecimal address going up to 16-bit integers.

**Ricoh 2CO2 NTSC/PAL** - Pixel processing unit(primitive graphics card) used for displaying and processing pixels.Depending on the region of the world,it has either NTSC port for NTSC TVs or PAL one. Its 3x faster than CPU and timing sensitive with many quirks.

**APU** - Audio processing unit that uses 5 channels that process audio samples that get passed to the TV so it can make a sound.The audio channels:Pulse 1+2, Triangle, Noise, DMC;

**Cartridge slot** - Inside of it it has the reader for the external cartridge and mappers.It reads the raw binary,uses the external Work Ram if exists and it can have 300+ mappers.Mappers are used for mapping the memory,memory mirrors and layouts of the game cuz of the small RAM of the NES.

**2KB RAM memory**

**4KB VRAM memory**

# Comparison to a real NES console

## NES vs NES-Emulator Components

| Component         | Real NES                        | NEScape                     |
|------------------|---------------------------------|---------------------------------|
| CPU               | Ricoh 2A03, 151 official opcodes (D flag ignored) | 151 official opcodes implemented |
| PPU (Graphics)    | 100% accurate, full NTSC/PAL quirks | ~75% accuracy goal, will handle most graphics but skips minor timing quirks |
| APU (Audio)       | 5 channels (Pulse 1+2, Triangle, Noise, DMC) | All 5 channels,presented by SDL |
| RAM               | 2 KB internal                            | 2 KB virtual                          |
| VRAM              | 4 KB internal                           | 4 KB virtual                           |
| Cartridge Slot    | Supports 300+ mappers           | Supports main mappers, limited subset(NROM,CNROM,UXROM,MMC1,MMC3) |
| Controllers       | 2 standard controllers + optional accessories (Zapper, etc.) | Controls will be mapped to keyboard and modern controller

# Early development

In the mid-July 2026,NEScape finally came to life and showed some graphics.It was an early test ROM with basic graphical output that was far from perfect but it was a huge milestone towards development.


<img width="775" height="748" alt="image" src="https://github.com/user-attachments/assets/93118d2e-ed6a-479c-ac6b-163cd8a9323a" />

<img width="782" height="753" alt="image" src="https://github.com/user-attachments/assets/735fd7a1-4de0-42ba-86af-f75cb7bd8e6f" />

# Goals for the future

I am happy to announce that this emulator is done at it's core but still it is not qualified yet for a full release.For a full 1.0.0 version,i need to implement:
- Accurate CPU timing and sprite zero hit corrections
- Finish the configuration,as it is a bare file with controller functionality only
- Optimize the audio,as it currently uses the expensive ring buffer,slowing emulation in some cases
- Proper User Interface,which supports loading previous games without touching the bare .nes files
- Save states,as currently save states or battery-backed saving is not yet supported
- Proper database of all games that this emulator can run
- Rewrite of architecture of specific parts to hit better accuracy of this legacy hardware
- Corrections in terms of AxROM mapper issue 
- Full support of Zapper,so we can further support Duck Hunt specifically

After the implementation of needed features and correction of the emulator to match the hardware closer,for future versions I will implement a series of compentent features that could push this software into a territory of serious products:
- Website where people interested in this type of software can browse the supported database,hardware documentation and be able to properly download this product.
-I will write a proper documentation which will cover this specific emulator and explain it how it works at it's core
- Push the commercial game support to at least 95%
- Invest serious time into developing a proper ecosystem of development tools and monitoring features regarding the work of this emulator

# ROM support
Rough estimate is 83% of commercial games that will boot at least,but current estimate of the playable games is yet to be discovered.When the full release come,please check the database that will cover the entire support.

# Operating system support
**NEScape** currently is supported only on **Windows** and will probably stay on it for quite some time if I stay the only contributor.I am planning the support to Linux but it is not one of the primary goals,especially not for a full release.
