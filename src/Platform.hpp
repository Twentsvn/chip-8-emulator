#pragma once

#include <cstdint>
#include <SDL.h>


class Platform
{

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
