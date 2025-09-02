# Chip8 Interpreter

## How to build
```bash
cd /path/to/chip-8-interpreter
mkdir build
cd build
cmake ..
cmake --build .
./chip8 <scale> <delay> <rom>
```

- Two test files are provided for testing the interpreter. 

## Resources
- https://github.com/dmatlack/chip8/tree/master/roms/games     ``For test rom``
- https://austinmorlan.com/posts/chip8_emulator/     ``For reference``


### NOTE
Our program runs the instructions a chip8 (no such hardware exist its just an interpreted programming language) would have run if provided to its cpu. I have tried SDL for rendering and sound could have also used just CLI for display but that would have nothing new.


