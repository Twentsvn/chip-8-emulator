#pragma once

#include <cstdint>
#include <random>

const unsigned int KEY_COUNT = 16; // 16 keys to interact
const unsigned int MEMORY_SIZE = 4096; // bytes
const unsigned int REGISTER_COUNT = 16;
const unsigned int STACK_LEVELS = 16;
const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;


class Chip8 {

    public:
        Chip8();
        void LoadROM(char const * filename);
        void Cycle();

        uint8_t keypad[KEY_COUNT]{};
        // a single pixel is 32 bits (4 bytes) and there are 64 x 32 such pixels which form the display
        // we are taking 32 bit pixel to make SDL easy
        uint32_t video[VIDEO_WIDTH * VIDEO_HEIGHT]{};
         uint8_t soundTimer{};

    private:
        void Table0();
        void Table8();
        void TableE();
        void TableF();

        // does nothing
        void OP_NULL();

        // CLS
        void OP_00E0();

        // RET
        void OP_00EE();

        // JP address
        void OP_1nnn();

        // CALL address
        void OP_2nnn();

        // SE Vx , byte
        void OP_3xkk();

        // SNE Vx , byte
        void OP_4xkk();

        // SE Vx , Vy
        void OP_5xy0();

        // LD Vx , byte
        void OP_6xkk();

        // ADD Vx , byte
        void OP_7xkk();

        // LD Vx , Vy
        void OP_8xy0();

        // OR Vx , Vy
        void OP_8xy1();

        // AND Vx , Vy
        void OP_8xy2();

        // XOR Vx , Vy
        void OP_8xy3();

        // ADD Vx , Vy
        void OP_8xy4();

        // SUB Vx , Vy
        void OP_8xy5();

        // SHR VX
        void OP_8xy6();

        // SUBN Vx , Vy
        void OP_8xy7();

        // SHL Vx
        void OP_8xyE();

        // SNE Vx , Vy
        void OP_9xy0();

        // LD I , address
        void OP_Annn();

        // JP V0 , address 
        void OP_Bnnn();

        // RND Vx , byte 
        void OP_Cxkk();

        // DRW Vx , Vy , height
        void OP_Dxyn();

        // SKP Vx
        void OP_Ex9E();

        // SKNP Vx
        void OP_ExA1();

        // LD Vx , DT
        void OP_Fx07();

        // LD Vx , K
        void OP_Fx0A();

        // LD DT , Vx
        void OP_Fx15();

        // LD ST , Vx
        void OP_Fx18();

        // ADD I , Vx
        void OP_Fx1E();

        // LD F , Vx
        void OP_Fx29();

        // LD B , Vx
        void OP_Fx33();

        // LD [I] , Vx
        void OP_Fx55();

        // LD Vx , [I]
        void OP_Fx65();


        uint8_t memory[MEMORY_SIZE]{};  // bytes accessable (addressable)
        uint8_t registers[REGISTER_COUNT]{};
        uint16_t index{};
        uint16_t pc{};  // memory address highest is 0xFFF which is 12 bits so we need 16 bits storage minimum
        uint8_t delayTimer{};
       
        uint16_t stack[STACK_LEVELS]{}; // stack will have memory address so 16 bits is minimum
        uint8_t sp{};  // its more like index to stack so 8 bits is more than enough since we will only be using 4 bits 0xF (16)
        uint16_t opcode{};

        std::default_random_engine randGen;
        std::uniform_int_distribution<uint8_t> randByte;

        typedef void (Chip8::*Chip8Func)();   // pointer to a member function for the Chip8 class
        Chip8Func table[0xF + 1];       // we are alocating more space than we needed here !!
        Chip8Func table0[0xE + 1];
        Chip8Func table8[0xE + 1];
        Chip8Func tableE[0xE + 1];
        Chip8Func tableF[0x65 +1];



};