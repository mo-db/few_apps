#define MA_NO_DECODING
#define MA_NO_ENCODING
#define MINIAUDIO_IMPLEMENTATION
#include "../libs/miniaudio/miniaudio.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>


#ifdef __EMSCRIPTEN__
#include <emscripten.h>

void main_loop__em()
{
}
#endif

#define DEVICE_FORMAT       ma_format_f32
#define DEVICE_CHANNELS     2
#define DEVICE_SAMPLE_RATE  48000

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    ma_waveform* pSineWave;

    MA_ASSERT(pDevice->playback.channels == DEVICE_CHANNELS);

    pSineWave = (ma_waveform*)pDevice->pUserData;
    MA_ASSERT(pSineWave != NULL);

    ma_waveform_read_pcm_frames(pSineWave, pOutput, frameCount, NULL);

    (void)pInput;   /* Unused. */
}

int main (int argc, char** argv)
{
    // MINIAUDIO
    ma_waveform sineWave;
    ma_device_config deviceConfig;
    ma_device device;
    ma_waveform_config sineWaveConfig;

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = DEVICE_FORMAT;
    deviceConfig.playback.channels = DEVICE_CHANNELS;
    deviceConfig.sampleRate        = DEVICE_SAMPLE_RATE;
    deviceConfig.dataCallback      = data_callback;
    deviceConfig.pUserData         = &sineWave;

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        printf("Failed to open playback device.\n");
        return -4;
    }

    printf("Device Name: %s\n", device.playback.name);

    sineWaveConfig = ma_waveform_config_init(device.playback.format, device.playback.channels, device.sampleRate, ma_waveform_type_sine, 0.2, 220);
    ma_waveform_init(&sineWaveConfig, &sineWave);

    if (ma_device_start(&device) != MA_SUCCESS) {
        printf("Failed to start playback device.\n");
        ma_device_uninit(&device);
        return -5;
    }
    
    // SDL
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    int result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    if (result < 0) {
        SDL_Log("SDL_Init error: %s", SDL_GetError());
    }

    window = SDL_CreateWindow("My WINDOW!", 640, 420, 0);
    if (window < 0) {
        SDL_Log("SDL_CreateWindow error: %s", SDL_GetError());
    }

    renderer = SDL_CreateRenderer(window, NULL);
    if (renderer < 0) {
        SDL_Log("SDL_CreateRenderer error: %s", SDL_GetError());
    }
    
    SDL_Log("sdl3 initialised!");

    SDL_Event event;
    int quit = 0;
    while (!quit) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
            case SDL_EVENT_QUIT:
                SDL_Log("sdl3 event quit!");
                quit = 1;
                break;
            }
            printf("tut\n");
            SDL_SetRenderDrawColorFloat(renderer, 0.0, 0.0, 0xff, 0xff);
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);
            SDL_Delay(1);
        }
    }

    SDL_Log("sdl3 shutdown.");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop__em, 0, 1);
#else
    printf("Press Enter to quit...\n");
    getchar();
#endif
    
    ma_device_uninit(&device);
    ma_waveform_uninit(&sineWave);  /* Uninitialize the waveform after the device so we don't pull it from under the device while it's being reference in the data callback. */
    
    (void)argc;
    (void)argv;
    return 0;
}
