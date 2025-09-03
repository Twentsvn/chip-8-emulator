#include "Platform.hpp"

#include <SDL.h>

void audio_callback(void * userdata , Uint8* stream , int len);


bool g_beep = false;

Platform::Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight)
{   
    // initialising sdl video and audio subsystem
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

	// creates a window we gives it a title , position and size
	window = SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		windowWidth, windowHeight,
		SDL_WINDOW_RESIZABLE);

    // rendering engine 
    renderer = SDL_CreateRenderer(window , -1 , SDL_RENDERER_ACCELERATED);

    // blank tecture
    texture = SDL_CreateTexture(
        renderer ,
        SDL_PIXELFORMAT_RGBA8888, // pixel 4 byte
        SDL_TEXTUREACCESS_STREAMING,
        textureWidth , textureHeight
    );

    SDL_AudioSpec want , have;
    SDL_zero(want);

    want.freq = 44100;
    want.format = AUDIO_S16SYS; // 16 bit signed sample
    want.channels = 1;
    want.samples = 4096;
    want.callback = audio_callback;

    m_audio_device = SDL_OpenAudioDevice(NULL, 0 , &want , &have , 0);
    SDL_PauseAudioDevice(m_audio_device,0);

}

Platform::~Platform()
{
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
    SDL_CloseAudioDevice(m_audio_device);
	SDL_Quit();
}

void Platform::Update(void const* buffer, int pitch)
{   
    // update 
	SDL_UpdateTexture(texture, nullptr, buffer, pitch);
	
    // clear the window
    SDL_RenderClear(renderer);

    // copy the texture and draw it onto the window
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
	
    // show on screen
    SDL_RenderPresent(renderer);
}

bool Platform::ProcessInput(uint8_t* keys)
{
	bool quit = false;

	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
        if (event.type == SDL_QUIT){
            quit = true;
        }

        if(event.type == SDL_KEYDOWN){
            if(event.key.keysym.sym == SDLK_ESCAPE){
                quit = true;
            }

            auto it = keymap.find(event.key.keysym.sym);
            if(it != keymap.end()){
                keys[it->second] = 1;
            }
        }

        if(event.type == SDL_KEYUP){
            auto it = keymap.find(event.key.keysym.sym);
            if (it != keymap.end()){
                keys[it->second] = 0;
            }
        }
    }
    return quit;
}

void audio_callback(void * userdata , Uint8 * stream , int len){
    Sint16 * buffer = (Sint16*)stream;
    int length = len / 2 ;

    static double phase = 0.0 ; //  
    double phase_step = 440.0 / 44100.0 ;

    for (int i=0 ; i < length ; ++i){
        if(g_beep){
            buffer[i] = (phase < 0.5) ? 1000 : -1000;
        }
        else {
            buffer[i] = 0;
        }

        phase += phase_step;
        if(phase >= 1.0){
            phase -= 1.0;
        }
    }
}