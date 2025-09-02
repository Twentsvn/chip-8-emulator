# Chip8 Emulator

## How to build
```bash
cd /path/to/chip-8-emulator
mkdir build
cd build
cmake ..
cmake --build .
./chip8 <scale> <delay> <rom>
```

- Two test files are provided for testing the emulator. 

### NOTE
Our program runs the instructions a chip8 (no such hardware exist its just an interpreted programming language) would have run if provided to its cpu. I have tried SDL for rendering and sound could hav also used just CLI for display but that would have nothing new.

