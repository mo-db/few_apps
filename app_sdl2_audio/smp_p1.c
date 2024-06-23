#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_stdinc.h>

static SDL_AudioDeviceID audio_device = 0;

int main(int argc, char **argv)
{
	int result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	if (result < 0) {
		SDL_Log("SDL_Init error: %s", SDL_GetError());
	}

	SDL_AudioSpec wavspec;
	Uint8 *wavbuf = NULL;
	Uint32 wavlen = 0;
	if (SDL_LoadWAV("CantinaBand60.wav", &wavspec, &wavbuf, &wavlen) == NULL) {
		fprintf(stderr, "coulndn't load wav file! %s", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	// FIXME: use later!
    SDL_AudioSpec desired;
    SDL_zero(desired);
    desired.format = AUDIO_F32;
    desired.channels = 2;
    desired.freq = 44100;
    desired.samples = 4096;
    desired.callback = NULL;
    //desired.userdata = &my_audio_callback_user_data;

	audio_device = SDL_OpenAudioDevice(NULL, 0, &wavspec, NULL, 0);
	// FIXME: error checking!

	SDL_QueueAudio(audio_device, wavbuf, wavlen);
	SDL_FreeWAV(wavbuf); // no longer in SDL3
	SDL_PauseAudioDevice(audio_device, 0); // device start pause -> unpause like this
	
	SDL_Delay(5000);

	SDL_CloseAudioDevice(audio_device);
	SDL_Quit();
	return 0;
}
