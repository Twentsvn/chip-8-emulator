#include "CLIdisplay.hpp"

#include "Chip8.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <chrono>
#include <thread>

#ifdef _WIN32
    #include <conio.h>
#else
    #include <ncurses.h>
#endif

extern const unsigned int VIDEO_HEIGHT;
extern const unsigned int VIDEO_WIDTH;
extern const unsigned int KEY_COUNT;

void CLIdisplay::cleardisplay(){
    #ifdef _WIN32
    std::cout << "\033[H\033[J"<<std::flush;
    
    #else
        clear();
        refresh();
    #endif
}


void CLIdisplay::clidisplay(uint32_t const * buffer){
    // we know buffer compromises of 4 bytes and we set 0xFFFFFFFF if pixel is ON
    // display is of 64 x 32 (VIDEO_WIDTH x VIDEO_HEIGHT)
   // std::string pixoff(videoScale , ' ');
   // std::string pixon(videoScale , '#');
    std::stringstream ss;
    std::string boundary(VIDEO_WIDTH+2 , '=');
    
    for(unsigned int h = 0 ; h < VIDEO_HEIGHT ; ++h ){
    // for (int i =0 ; i < videoScale ; ++i){
        if(h==0){
            ss <<"  " << boundary << "\r\n";
        }

      for (unsigned int w=0 ; w < VIDEO_WIDTH ; ++w){
        if(w==0) ss<< "  |  ";
        if(buffer[ h * VIDEO_WIDTH + w] == 0xFFFFFFFF) {
            ss << '#';
            //ss << "1";
            }
        else{
            ss << ' ';
            //ss << "0";
        } 
    } 
    ss << "|\r\n";
    }
    
    ss << "  "<< boundary;
    
    std::cout << ss.str() << std::flush ;
     
    //ss.str("");
    //std::this_thread::sleep_for(std::chrono::milliseconds(16));
    
}

bool CLIdisplay::ProcessInput(uint8_t * keys){
    static int cycledecay {1}; // identity of cycledecay need to be mainted for future calls
    // press downn all
    cycledecay = cycledecay > 0 ? cycledecay-1 : cycledecay;
    
    bool quit = false;
    char  keyPressed ;
    
    if( cycledecay == 0)  // sustaining the input is necessary 
    for (unsigned int i=0; i < 16 ; ++i){
        keys[i]=0;
    }
    
// if though cycledecay is not zero but we can take more inputs which are necessary for 2 player games
    

#ifdef _WIN32
    if (_kbhit()){  // returns zero if no value pressed
        keyPressed = _getch();
          if(keyPressed == 'p') {
             quit = true ;
         }
        auto it = Clikeymap.find(keyPressed);
        if(it != Clikeymap.end()){ 
            keys[it->second] = 1;
            cycledecay = 12;
        }

    }
#else
    napms(2);  // used dor sleep in milisecond
    keyPressed = getch();
    
    if( keyPressed != ERR ){
       
       // printw("%c",keyPressed); refresh();
         if(keyPressed == 'p') {
             quit = true ;
         }
       // std::cout << "Pressed: " << keyPressed << " (" << static_cast<char>(keyPressed) << ")\r\n";
        auto it = Clikeymap.find(keyPressed);
        //std::cout << static_cast<char>(keyPressed);
        if(it != Clikeymap.end()){ // keymap is mapping of keyboard keys to 0x0 --> 0xF
            keys[it->second] = 1;
            cycledecay = 12;
        }
       // PrintKeys(keys)
        
    }
    

#endif

    return quit;
}

void PrintKeys(const uint8_t* keys) {   // using cout in ncurses is bad since it distortes the display!!
    std::cout << "Chip-8 Keys State:\r\n";
    std::cout << "[1] [2] [3] [C]\r\n";
    printf(" %d   %d   %d   %d\r\n", keys[0x1], keys[0x2], keys[0x3], keys[0xC]);
    std::cout << "[4] [5] [6] [D]\r\n";
    printf(" %d   %d   %d   %d\r\n", keys[0x4], keys[0x5], keys[0x6], keys[0xD]);
    std::cout << "[7] [8] [9] [E]\r\n";
    printf(" %d   %d   %d   %d\r\n", keys[0x7], keys[0x8], keys[0x9], keys[0xE]);
    std::cout << "[A] [0] [B] [F]\r\n";
    printf(" %d   %d   %d   %d\r\n", keys[0xA], keys[0x0], keys[0xB], keys[0xF]);
    std::cout << "\r\nPress a mapped key (1-4, q-r, a-f, z-v) or 'p'/ESC to quit.\r\n";
};

