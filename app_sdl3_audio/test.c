#include <SDL3/SDL.h>
#include <math.h>
#include <stdbool.h>
#define PI2 6.28318530718

int main(int argc, char **argv)
{
    int result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    if (result < 0) {
        SDL_Log("SDL_Init error: %s", SDL_GetError());
        return 1;
    }

    const SDL_AudioSpec spec = { SDL_AUDIO_S16, 1, 44100 };
    SDL_AudioStream *stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);

    Sint8 buf[88200];
    Uint32 buf_len = 0;
    float freq = 59;
    float time = 0;

    bool running = true;
    bool play_waveform = false;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            if (event.type == SDL_EVENT_KEY_DOWN) {
                play_waveform = true;
            }
        }

        if (play_waveform) {
            buf_len = sizeof(buf);
            for (int i = 0; i < buf_len; i++) {
                buf[i] = 255.0 / 2.0 + (255.0 / 2.0 * sin(time));
                if (buf[i] % 5 != 0) {
                    buf[i] /= 8;
                }

                time += freq * PI2 / 44100.0;
                if (time >= PI2)
                    time -= PI2;

                printf("%d, \n", buf[i]);
            }
            printf("len: %d\n", buf_len);

            SDL_PutAudioStreamData(stream, buf, buf_len);
            SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(stream));
            
            play_waveform = false; // Reset the flag to wait for the next key press
        }

        SDL_Delay(10); // Small delay to avoid high CPU usage
    }

    SDL_Quit();
    return 0;
}
