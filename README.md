# nes
NES emulator written in C++

## Status
Most of the CPU has been implemented.
The PPU only works for very basic games that don't use scrolling (e.g. Donkey Kong).
Anything more complex will probably have graphical glitches.
Only NROM (mapper 0) is supported.

## Building

You'll need the following to build:

- A C++11 compiler
- SDL2

## Usage

	nes <ROM filename>

will run the specified ROM

## Controls (Hardcoded)
A - X

B - Z

D-Pad - Arrow Keys

Select - Backspace

Start - Enter
