#pragma once 
#include <unordered_map>
#ifdef _WIN32
    #include <conio.h>
#else
    #include <ncurses.h>
#endif

const std::unordered_map<char , int> Clikeymap {
    {'1', 0x1} , {'2' , 0x2} , {'3' , 0x3} , {'4', 0xC},
    {'q', 0x4} , {'w' , 0x5} , {'e' , 0x6} , {'r', 0xD},
    {'a', 0x7} , {'s' , 0x8} , {'d' , 0x9} , {'f', 0xE},
    {'z', 0xA} , {'x' , 0x0} , {'c' , 0xB} , {'v', 0xF},
    
};

void PrintKeys(const uint8_t * keys);

class CLIdisplay {
    
    public:
    static void clidisplay(uint32_t const * buffer);
    static void cleardisplay();
    static bool ProcessInput(uint8_t * keys );
};