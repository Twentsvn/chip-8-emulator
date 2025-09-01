#include "Chip8.hpp"
#include "Platform.hpp"
#include <chrono>
#include <iostream>

extern bool g_beep ;

int main(int argc , char ** argv){

    if (argc != 4){
        std::cerr << "Usage : " << argv[0] << " <Scale> <Delay> <ROM> \n";
        std::exit(EXIT_FAILURE);
    }

    int videoScale = std::stoi(argv[1]);
    int cycleDelay = std::stoi(argv[2]);
    char const * romFilename = argv[3];

    Platform platform("CHIP-8 Emulator" , VIDEO_WIDTH * videoScale , VIDEO_HEIGHT * videoScale , VIDEO_WIDTH , VIDEO_HEIGHT);

    Chip8 chip8;
    // load the rom file
    chip8.LoadROM(romFilename);

    int videoPitch = sizeof(chip8.video[0])* VIDEO_WIDTH ;

    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool quit = false;

    while(!quit){
        // which key pressed 
        quit = platform.ProcessInput(chip8.keypad);

        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float , std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

        if(dt > cycleDelay){ // only run after cycle delay 
            lastCycleTime = currentTime;
            chip8.Cycle();
            
            if(chip8.soundTimer > 0){
                g_beep = true;
            }
            else {
                g_beep = false;
            }

            platform.Update(chip8.video , videoPitch);
            
        }

    }

    return 0;
}