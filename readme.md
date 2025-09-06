# Chip8 Interpreter

## How to build
```bash
cd /path/to/chip-8-interpreter
mkdir build
cd build
cmake ..
cmake --build .
./chip8 <displayType> <scale> <delay> <rom>
```

``displayType = 1 => SDL``  \
``displayType = 0 => CLI``

- Two test files are provided for testing the interpreter. 

### Works on both Windows and Linux

## Resources
- https://github.com/dmatlack/chip8/tree/master/roms/games     ``For test rom``
- https://austinmorlan.com/posts/chip8_emulator/     ``For reference``


### NOTE
- Our program runs the instructions a chip8 (no such hardware exist its just an interpreted programming language) would have run if provided to its cpu which does not exist.
- I have tried SDL for rendering and sound
- I have also used CLI for display and did not try for sound.


