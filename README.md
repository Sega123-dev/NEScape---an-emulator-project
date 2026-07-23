# NES-Emulator

# What is an emulator?

Emulator is a software that mimics the real hardware.This repository is example of an emulator based on a legacy 8-bit console manufactured in 1983 called **Nintendo Entertainment System(NES)** in which development,various companies worked,mainly Nintendo.Like other emulators,this emulator is also not 100% accurate as real console.In theory the accuracy of this emulator is around 65% if we do not include the pirated and homebrew aspects of this console.With this emulator in theory,you should be able to play around 70% of commercial titles.
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

| Component         | Real NES                        | NES Emulator                     |
|------------------|---------------------------------|---------------------------------|
| CPU               | Ricoh 2A03, 151 official opcodes (D flag ignored) | Full official CPU, 149 opcodes, excluding D-flag handling |
| PPU (Graphics)    | 100% accurate, full NTSC/PAL quirks | ~75% accuracy, handles most graphics but skips minor timing quirks |
| APU (Audio)       | 5 channels (Pulse 1+2, Triangle, Noise, DMC) | 3 easiest channels implemented(Pulse 1 and 2,Triangle) |
| RAM               | 2 KB                            | 2 KB                            |
| VRAM              | 4 KB                            | 4 KB                            |
| Cartridge Slot    | Supports 300+ mappers           | Supports main mappers, limited subset(NROM,CNROM,UXROM,MMC1,MMC3) |
| Controllers       | 2 standard controllers + optional accessories (Zapper, etc.) | Controls mapped to keyboard

# Early development

In the mid-July 2026,NEScape finally came to life and showed some graphics.It was an early test ROM with basic graphical output that was far from perfect but it was a huge milestone towards development.
<img width="817" height="779" alt="image" src="https://github.com/user-attachments/assets/9f2165cb-c632-497d-b8e8-85e885e6134b" />


# Support and versions

- *v0.1* - Unstable and unplayable version.
- *v0.5* - First MVP.

Emulator currenlty can work only on **Windows** OS.
