#include <SDL3/SDL.h>
#include <stdlib.h>
#include <math.h>
#define PI2 6.28318530718

static struct {
		
}

static void panic_and_abort(const char *title, const char *text)
{/*{{{*/
	fprintf(stderr, "PANIC: %s ... %s\n", title, text);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, text, NULL); // if window exists attach to window
	SDL_Quit(); // It's allways save to call, even if not initialised before!
	exit(1);
}/*}}}*/

int main(int argc, char **argv)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
		panic_and_abort("SDL_Init error!", SDL_GetError());
	}

	// Video stuff
	window = SDL_CreateWindow("Hello SDL!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640);
	if (!window) {
		panic_and_abort("SDL_CreateWindow failed!", SDL_GetError());
	}

	// SDL_RENDERER_PRESENTVSYNC -> will wait at present function to sync frames to display
	// so the loop doesn't run at max cpu speed
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) {
		panic_and_abort("SDL_CreateRenderer failed!", SDL_GetError());
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
