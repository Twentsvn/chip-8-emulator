#include "Chip8.hpp"
#include "Platform.hpp"
#include "CLIdisplay.hpp"
#include <chrono>
#include <iostream>
#include <thread>
#include <memory>

extern bool g_beep ;

int main(int argc , char ** argv){

    if (argc != 5){
        std::cerr << "Usage : " << argv[0] << "<DisplayType> <Scale> <Delay> <ROM> \n CLI = 0 , Rendered Display = 1 \n ";
        std::exit(EXIT_FAILURE);
    }
    
    int disType ;
    int videoScale ;
    int cycleDelay ;
    char const * romFilename ;

    try{
     disType = std::stoi(argv[1]);
     videoScale = std::stoi(argv[2]);
     cycleDelay = std::stoi(argv[3]);
     romFilename = argv[4];
    }
    catch(const std::exception &e){
        std::cerr << e.what() << std::flush ;
        return -1;
    }

    if(disType > 1 || disType < 0 ){std::cerr << "Invalid <DisplayType>. Please use 0 or 1."; std::exit(EXIT_FAILURE);}

    #ifndef _WIN32
        initscr();  // start ncurses
        //raw();
        cbreak();
        noecho();   // dont print typed char
        nodelay(stdscr , TRUE);     // make getch() non-blocking
       // timeout(cycleDelay*2); // milisecond
        curs_set(0);    // hide the cursor
    #endif


    std::unique_ptr<Platform> platform; //

    if(disType == 1){
        try{
    platform = std::make_unique<Platform>("CHIP-8 Emulator" , VIDEO_WIDTH * videoScale , VIDEO_HEIGHT * videoScale , VIDEO_WIDTH , VIDEO_HEIGHT);
        }
        catch(const std::exception &e) {std::cerr << e.what()<<std::endl;}
    }
    Chip8 chip8;
    // load the rom file
    chip8.LoadROM(romFilename);

    int videoPitch = sizeof(chip8.video[0])* VIDEO_WIDTH ;

   // auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool quit = false;

    while(!quit){
        // which key pressed
         
        if(disType == 1){
        quit = platform->ProcessInput(chip8.keypad);
        }
        else{
            quit = CLIdisplay::ProcessInput(chip8.keypad);
        }
       
        std::this_thread::sleep_for(std::chrono::milliseconds(cycleDelay)); // we can do #include <windows.h> and do Sleep(cycleDelay); for same for windows ofc 

             // only run after cycle delay 
            //lastCycleTime = currentTime;
            chip8.Cycle();
            
            if(disType==1){
                if(chip8.soundTimer > 0){
                    g_beep = true;
                }
                else {
                    g_beep = false;
                }

                platform->Update(chip8.video , videoPitch);
             }
            else{

                CLIdisplay::cleardisplay();
                CLIdisplay::clidisplay(chip8.video);

               // PrintKeys(chip8.keypad);
            }
        

    }

    #ifndef _WIN32
        endwin(); // ncurses cleanup
    #endif

    return 0;
}