#include "Chip8.hpp"
#include <chrono>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <random>

const unsigned int FONTSET_SIZE = 80; // 16 * 5(bits)
const unsigned int FONTSET_START_ADDRESS = 0x50;
const unsigned int START_ADDRESS = 0x200;

uint8_t fontset[FONTSET_SIZE] = {

        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8()
    : randGen(std::chrono::system_clock::now().time_since_epoch().count()){
        
        // Initialize PC (Program counter)
        pc = START_ADDRESS ; 

        // load fonts in memory
        for (unsigned int i = 0 ; i < FONTSET_SIZE ; ++i){

            memory[FONTSET_START_ADDRESS + i] = fontset[i];

        }

        randByte = std::uniform_int_distribution<uint8_t>(0,255U);

        // set up function pointer table
        table[0x0] = &Chip8::Table0;  // Table0 is function and table0 is the data structure
        table[0x1] = &Chip8::OP_1nnn;
        table[0x2] = &Chip8::OP_2nnn;
        table[0x3] = &Chip8::OP_3xkk;
        table[0x4] = &Chip8::OP_4xkk;
        table[0x5] = &Chip8::OP_5xy0;
        table[0x6] = &Chip8::OP_6xkk;
        table[0x7] = &Chip8::OP_7xkk;
        table[0x8] = &Chip8::Table8; // there are multiple instruction starting from 8
        table[0x9] = &Chip8::OP_9xy0;
        table[0xA] = &Chip8::OP_Annn;
        table[0xB] = &Chip8::OP_Bnnn;
        table[0xC] = &Chip8::OP_Cxkk;
        table[0xD] = &Chip8::OP_Dxyn;
        table[0xE] = &Chip8::TableE; // same 
        table[0xF] = &Chip8::TableF; // same

        // initialising entries to OP_NULL , since they are limited instructions
        for (size_t i = 0 ; i <=0xE ; ++i){
            table0[i] = &Chip8::OP_NULL;
            table8[i] = &Chip8::OP_NULL;
            tableE[i] = &Chip8::OP_NULL;
        }

        table0[0x0] = &Chip8::OP_00E0;
        table0[0xE] = &Chip8::OP_00EE;
        // finished instruction starting from zero

        table8[0x0] = &Chip8::OP_8xy0;
        table8[0x1] = &Chip8::OP_8xy1;
        table8[0x2] = &Chip8::OP_8xy2;
        table8[0x3] = &Chip8::OP_8xy3;
        table8[0x4] = &Chip8::OP_8xy4;
        table8[0x5] = &Chip8::OP_8xy5;
        table8[0x6] = &Chip8::OP_8xy6;
        table8[0x7] = &Chip8::OP_8xy7;
        table8[0xE] = &Chip8::OP_8xyE;
        // finished instruction starting from 8

        tableE[0x1] = &Chip8::OP_ExA1;
        tableE[0xE] = &Chip8::OP_Ex9E;

        // our tableF has more than extra entries but if there were many instruction not like chip8
        for(size_t i = 0 ; i <= 0x65 ; ++i){

            tableF[i]=&Chip8::OP_NULL;

        }

        tableF[0x07] = &Chip8::OP_Fx07;
        tableF[0x0A] = &Chip8::OP_Fx0A;
        tableF[0x0A] = &Chip8::OP_Fx15;
        tableF[0x18] = &Chip8::OP_Fx18;
        tableF[0x1E] = &Chip8::OP_Fx1E;
        tableF[0x29] = &Chip8::OP_Fx29;
        tableF[0x33] = &Chip8::OP_Fx33;
        tableF[0x55] = &Chip8::OP_Fx55;
        tableF[0x65] = &Chip8::OP_Fx65;

    }


void Chip8::LoadROM(char const * filename){
    // rom opened as stream
    std::ifstream file(filename , std::ios::binary | std::ios::ate);

    if (file.is_open()){
        // size of file and buffer of that size
        std::streampos size = file.tellg();
        char * buffer = new char [size];

        file.seekg(0,std::ios::beg); // 0th byte (--> offset) from beginning of stream
        file.read(buffer,size); // extracts 'size' bytes or characters from the stream and store at buffer    
        file.close();

        // load the contents at 0x200 
        for(long i = 0 ; i<size ; ++i){
            memory[START_ADDRESS+i] = buffer[i];
        }
        // free the buffer
        delete [] buffer;

    }

}

    /* ---- CORRECT FUNCTION CALL FOR OPCODE ------ */

void Chip8::Cycle(){
    // we can access bytes , our opcode is 2 bytes how to club 
    //them together, 0x1010 is our opcode so we can access
    // 0x10 0x10 but how to club them together one way is
    // 0x10*16 + 0x10 or 0x10 << 8u ==> (0x1000) OR 0x0010
    // which will be 0x1010 which we wanted 
    opcode = (memory[pc] << 8u) | memory[pc+1] ;
    pc+=2;

    // table is our function pointer table 
    // correct function call depends upon the highest nibble in opcode
    // like 0x1nnn so we need to call table[0x1] , we need highest nibble (4 bits)
    // 0x1nnn AND 0xF000 (F means al 4 bits are ones) so and will give 0x1000 and we need to 
    // 0x1000 >> 12u (2 bytes long 4 bits which we need so 12 bits we need to remove )

    ((*this).*(table[(opcode & 0xF000u) >> 12u]))(); // function call

    // each instruction is completed in a cycle and each cycle 
    // will decrement the timer 
    if (delayTimer > 0) --delayTimer;
    if ( soundTimer > 0) --soundTimer;
    
}

void Chip8::Table0(){
    // we need the first 4 bits 0x0001 this 4 bits which represnt one
    // 0x000Fu AND 0xabc1 => 0x0001 , we dont need to shift the bits since 0x0001 and 0x1 is same (our index) 
    ((*this).*(table0[opcode & 0x000Fu]))();
}

void Chip8::Table8(){
    // same first 4 bits
    ((*this).*(table8[opcode & 0x000Fu]))();
}

void Chip8::TableE(){
    // same first 4 bits
    ((*this).*(tableE[opcode & 0x000Fu]))();
}

void Chip8::TableF(){
    // last byte so 0xab20 => 0x00FF AND 0xab20 == 0x0020 
    ((*this).*(tableF[opcode & 0x00FFu]))();
}

   
 /* ------ OPCODE IMPLEMENTATION -------- */

void Chip8::OP_NULL(){
    // nothing empty cycle
}

void Chip8::OP_00E0(){ // clear the display
    memset(video , 0 , sizeof(video)); // set all to zeros means pixel OFF
}

void Chip8::OP_00EE(){ // return from subroutine 
    --sp; // sp points to next one
    pc = stack[sp]; 

}

void Chip8::OP_1nnn(){ // Jump to location nnn
    /*uint16_t address = opcode & 0x0FFFu ; // first 12 bits
    pc = address ; */ 
    pc = opcode & 0x0FFFu; // opcode 2 bytes and 0x0FFF also 2 bytes result 2 bytes and pc also 2 bytes

}

void Chip8::OP_2nnn(){ // Call subroutine at nnn
    /*  we need to put the current instruction to stack 
        and change the pc to call addrees which are first 12 bits
    */
    stack[sp] = pc;
    ++sp; // sp points to where we can insert new
    pc = opcode & 0x0FFFu ;

}

void Chip8::OP_3xkk(){ // skip next instruction if Vx = kk
    /* Vx is the 3rd nibble and kk is 1st byte*/
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // we need to shift it 8 bits
    uint8_t byte = (opcode & 0x00FFu);

    if(registers[Vx] == byte) pc+=2; // since our pc points to next instruction so we only need to +2
    
}

void Chip8::OP_4xkk(){ // skip next isntrution if Vx!=kk
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = (opcode & 0x00FFu);
    if(registers[Vx] != byte) pc+=2; 
}

void Chip8::OP_5xy0(){ // skip next instruction if Vx = Vy
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if(registers[Vx] == registers[Vy]) pc+=2;

}

void Chip8::OP_6xkk(){ // set Vx =kk
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = (opcode & 0x00FFu);

    registers[Vx] = byte;
}

void Chip8::OP_7xkk(){ // Vx = Vx + kk
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = (opcode & 0x00FFu);

    registers[Vx] += byte;
}

void Chip8::OP_8xy0(){ // set Vx = Vy
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    
    registers[Vx] = registers[Vy];
}

void Chip8::OP_8xy1(){ // set Vx = Vx OR Vy
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] |= registers[Vy] ;
}

void Chip8::OP_8xy2(){ // Vx = Vx + Vy
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] += registers[Vy];
}

void Chip8::OP_8xy3(){ // Vx = Vx XOR Vy
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] ^= registers[Vy];
}

void Chip8::OP_8xy4(){ // Vx = Vx + Vy , VF = carry
    /* the values are added together and if overflow occors VF is set to 1 
    otherwise 0. and only 8 bits are stored in Vx */
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    uint16_t sum = registers[Vx] + registers[Vy];

    if(sum > 255U){
        registers[0xF] = 1;
    }
    else{
        registers[0xF] = 0;
    }

    registers[Vx] = sum & 0x00FFu; // last 1 byte which we need

}

void Chip8::OP_8xy5(){ // Vx = Vx - Vy 
/* Vx > Vy ; VF = 1 otherwise 0  */
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if(registers[Vx] > registers[Vy]) {
        registers[0xF]=1;
    }
    else{
        registers[0xF]=0;
    }
    registers[Vx] -= registers[Vy];
}

void Chip8::OP_8xy6(){ // Vx = Vx SHR 1
    /* If least significant bit of Vx is 1 , then VF =1 otherwise 0 */

    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    registers[0xF] = (registers[Vx] & 0x01u);
    registers[Vx] >>= 1; // right shift 1 bit
}

void Chip8::OP_8xy7(){ // Vx = Vy - Vx
    /* Vy>Vx then Vf is set 1 otherwise 0*/
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if(registers[Vy]>registers[Vx]){
        registers[0xF] = 1;
    }
    else{
        registers[0xF] = 0;
    }

    registers[Vx] = registers[Vy] - registers[Vx];
}

void Chip8::OP_8xyE(){ // Vx = Vx SHL 1
    /* If MSB is 1 then VF=1 otherwise 0*/

    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    
    registers[0xF] = (registers[Vx] & 0x80) >> 7u ;

    registers[Vx] <<= 1;  
}

void Chip8::OP_9xy0(){ // skip next instruction if Vx != Vy

    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if(registers[Vx]!=registers[Vy]) pc+=2;
}

void Chip8::OP_Annn(){ // set I = nnn
    uint16_t address = opcode & 0x0FFFu ;

    index = address; 
}

void Chip8::OP_Bnnn(){ // JMP to location nnn + V0
    uint16_t address = opcode & 0x0FFFu ;
    
    pc = address + registers[0];
}

void Chip8::OP_Cxkk(){ // set Vx = random byte AND kk
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = (opcode & 0x00FFu);

    registers[Vx] = randByte(randGen) & byte ;
}

void Chip8::OP_Dxyn(){ // Display n-byte sprite starting at memory location I at (Vx,Vy), set VF=collision
    /* Sprites are 1 to 15 pixels high and 8 pixels wide */

    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint8_t height = (opcode & 0x000Fu) ;

    uint8_t xPos = registers[Vx] % VIDEO_WIDTH; // wraping if overflow
    uint8_t yPos = registers[Vy] % VIDEO_HEIGHT; // same

    registers[0xF]=0;
    
    for (unsigned int row = 0 ; row < height; ++ row){
        
        uint8_t spriteByte = memory[index + row]; 
                                   // 8 bits        
        for (unsigned int col = 0 ; col < 8 ; ++col){
            
            uint8_t spritePixel = spriteByte & (0x80u >> col); // 8 bits
            // screenpixel is the display pixel at 
            uint32_t * screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col) ];

            // sprite pixel is ON            
            if(spritePixel)
            {   
                // screen pixel is ON (since it is a 32 bit number thats why when its ON its 0xFFFFFFFF)
                if(*screenPixel == 0xFFFFFFFF){
                    // Collision
                    registers[0xF] = 1;
                }
                // xor the sprite and screen pixel
                *screenPixel ^= 0xFFFFFFFF; // sprite pixel is ON so XOR with 0xFFFFFFFF
            }
        }
    }

}

void Chip8::OP_Ex9E(){ // Skip next instruction if key with value of Vx is pressed

    uint8_t Vx = (opcode & 0x0F00u ) >> 8u;
    uint8_t key = registers[Vx];

    if(keypad[key]) pc+=2;

}

void Chip8::OP_ExA1(){ // Skip the next instruction if key with the value of Vx is not pressed
    uint8_t Vx = (opcode & 0x0F00u ) >> 8u;
    uint8_t key = registers[Vx];

    if(!keypad[key]) pc+=2;
}

void Chip8::OP_Fx07(){ // set Vx = delay timer value
    uint8_t Vx = (opcode & 0x0F00u ) >> 8u;
    
    registers[Vx] = delayTimer;
}

void Chip8::OP_Fx0A(){ // wait for a key press and store it in Vx
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    
    uint8_t check = 0; 
    for (unsigned int i = 0 ; i < 0xF ; ++i){
        if(keypad[i]){
            registers[Vx] = i;
            check = 1;
        }
    }
    if(!check) pc-=2;

}

void Chip8::OP_Fx15(){ // set delay timer = Vx
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    delayTimer = registers[Vx];
}

void Chip8::OP_Fx18(){ // set sound timer = Vx
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    soundTimer = registers[Vx];
}

void Chip8::OP_Fx1E(){ // I = I + Vx
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    index += registers[Vx];
}

void Chip8::OP_Fx29(){ // set I = location of sprite for digit Vx
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t digit = registers[Vx];
    
    // fonts are 5 byte in size
    index = FONTSET_START_ADDRESS + ( 5 * digit);
}

void Chip8::OP_Fx33(){ 
    /* Store BCD (binary coded decimal) representation of Vx in memory locations I , I+1 , I+2
     The interpreter takes the decimal value of Vx, and places the hundreds 
     digit in memory at location in I, the tens digit at location I+1, and 
     the ones digit at location I+2.
     each decimal digit (0-9) is represented by a 4 bit binary code*/
    uint8_t Vx = (opcode & 0x0F00) >> 8u;
    uint8_t value = registers[Vx];

    // ones-place
    memory[index + 2 ] = value % 10 ;
    value /=10;

    //tens-place
    memory[index + 1] = value % 10 ;
    value /=10;

    //hundreds place
    memory[index] = value % 10 ; 

}

void Chip8::OP_Fx55(){ // store registers V0 through Vx in memory starting at location I
    uint8_t Vx = (opcode & 0x0F00) >> 8u;

    for (uint8_t i = 0 ; i <= Vx ; ++i){
        memory[index + i] = registers[i];
    }
}

void Chip8::OP_Fx65(){ // read registers V0 though Vx from memory starting at location I
    uint8_t Vx = (opcode & 0x0F00) >> 8u;

    for (uint8_t i = 0 ; i <=Vx ; ++i ){
        registers[i] = memory[index + i];
    }

}