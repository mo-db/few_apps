#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_stdinc.h>
//#include <SDL2/SDL_audio.h>
//#include <SDL2/SDL_stdinc.h>
#include <math.h>
#define PI2 6.28318530718

static SDL_AudioDeviceID audio_device = 0;


int main(int argc, char **argv)
{
	int result01 = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	if (result01 < 0) {
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
    desired.format = AUDIO_S16SYS;
    desired.channels = 1;
    desired.freq = 44100;
    //desired.samples = 4096;
    desired.callback = NULL;
    //desired.userdata = &my_audio_callback_user_data;

// MY Testing Stuf
	Sint16 buf[88200*2];
	//Uint8 *buf = ori_buf;
	Uint32 buf_len = 0;
	float freq = 130;
	float time = 0;

	buf_len = sizeof(buf)/2;
	for(int i = 0; i < buf_len; i++) {
		buf[i] = 16000 * sin(time);
		
		time += freq * PI2 / 44100.0;
		if(time >= PI2)
			time -= PI2;

		printf("%d, \n", buf[i]);
	}
	printf("len: %d", buf_len);

	audio_device = SDL_OpenAudioDevice(NULL, 0, &desired, NULL, 0);
	// FIXME: error checking!

	SDL_QueueAudio(audio_device, buf, buf_len);
	SDL_FreeWAV(wavbuf); // no longer in SDL3
	SDL_PauseAudioDevice(audio_device, 0); // device start pause -> unpause like this
	
	SDL_Delay(5000);

	SDL_CloseAudioDevice(audio_device);
	SDL_Quit();
	return 0;
}
