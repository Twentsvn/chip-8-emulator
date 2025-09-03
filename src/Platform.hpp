#pragma once

#include <cstdint>
#include <SDL.h>
#include <unordered_map>

static const std::unordered_map<SDL_Keycode , int> keymap = {
    {SDLK_1 , 0x1} , {SDLK_2 , 0x2} , {SDLK_3 , 0x3} , {SDLK_4 , 0xC},
    {SDLK_q , 0x4} , {SDLK_w , 0x5} , {SDLK_e , 0x6} , {SDLK_r , 0xD},
    {SDLK_a , 0x7} , {SDLK_s , 0x8} , {SDLK_d , 0x9} , {SDLK_f , 0xE},
    {SDLK_z , 0xA} , {SDLK_x , 0x0} , {SDLK_c , 0xB} , {SDLK_v , 0xF}
 };
 class Platform{

    public:
        Platform(char const * title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
        ~Platform();
        void Update(void const * buffer , int pitch);
        bool ProcessInput(uint8_t * keys);
        
    private:
        SDL_Window * window{nullptr};
        SDL_AudioDeviceID m_audio_device;
        SDL_Renderer * renderer{nullptr};
        SDL_Texture * texture{nullptr};
};
