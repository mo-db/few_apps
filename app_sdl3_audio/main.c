#include <SDL3/SDL.h>
#include <math.h>
#define PI2 6.28318530718

int main(int argc, char **argv)
{
	int result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	if (result < 0) {
		SDL_Log("SDL_Init error: %s", SDL_GetError());
	}
	SDL_AudioSpec wavspec;
	Uint8 *wavbuf = NULL;
	Uint32 wavlen = 0;
	if (SDL_LoadWAV("CantinaBand60.wav", &wavspec, &wavbuf, &wavlen) > 0) {
		fprintf(stderr, "coulndn't load wav file! %s", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	const SDL_AudioSpec spec = { SDL_AUDIO_S16, 1, 44100 };
	SDL_AudioStream *stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);



	Sint8 buf[88200];
	Uint32 buf_len = 0;
	float freq = 59;
	float time = 0;

	buf_len = sizeof(buf);
	for(int i = 0; i < buf_len; i++) {
		buf[i] = 255.0/2.0 + (255.0/2.0 * sin(time));
		if (buf[i] % 5 != 0) {
			buf[i] /= 8;
		}
		
		time += freq * PI2 / 44100.0;
		if(time >= PI2)
			time -= PI2;

		printf("%d, \n", buf[i]);
	}
	printf("len: %d", buf_len);





	SDL_PutAudioStreamData(stream, buf, buf_len);
	SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(stream));

	SDL_Delay(5000);
	//printf("test: %lu \n", sizeof(wavbuf));
	//printf("wavspec: %us", wavspec.format);
#if 0
	for (int i = 0; i < wavlen; i += 3) {
		printf("%d, %d, %d\n", wavbuf[i], wavbuf[i + 1], wavbuf[i + 2]);
	}
#endif

	SDL_free(wavbuf);
	SDL_Quit();
	return 0;
}
