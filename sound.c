#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

char dirlist[]="data";

int readsound(int num);

#define NUMSOUNDS	(sizeof(soundnames)/sizeof(char*))
#define MIXMAX 16

#define SOUND_QUIET -1

char *soundnames[] =
{
"bomb1.wav",
"power1.wav",
"death.wav",
"drop.wav",
"bomb2.wav",
"power2.wav",
};

typedef struct sample
{
    Mix_Chunk *chunk;
    int len;
} sample;

#define SNDFRAGMENT 1024

sample samples[NUMSOUNDS];

int soundworking=0;

int soundopen(void)
{
    int i;

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return -1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
        fprintf(stderr, "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return -1;
    }

    for (i = 0; i < NUMSOUNDS; ++i) {
        if (readsound(i) != 0) {
            fprintf(stderr, "Failed to load sound %d\n", i);
            return -1;
        }
    }

    soundworking = 1;
    return 0;
}

void soundclose(void)
{
    int i;
    if (soundworking) {
        for (i = 0; i < NUMSOUNDS; ++i) {
            if (samples[i].chunk) {
                Mix_FreeChunk(samples[i].chunk);
                samples[i].chunk = NULL;
            }
        }
        Mix_CloseAudio();
        SDL_Quit();
        soundworking = 0;
    }
}


int readsound(int num)
{
    char name[256], *p1, *p2, ch;
    p1 = dirlist;
    for (;;) {
        p2 = name;
        while (*p1 && (ch = *p1++) != ',')
            *p2++ = ch;
        if (p2 > name && p2[-1] != '/')
            *p2++ = '/';
        strcpy(p2, soundnames[num]);
        
        samples[num].chunk = Mix_LoadWAV(name);
        if (samples[num].chunk) {
            samples[num].len = samples[num].chunk->alen;
            break;
        }
        if (!*p1) {
            fprintf(stderr, "Failed to load sound: %s\n", Mix_GetError());
            return -1;
        }
    }
    return 0;
}

void playsound(int n)
{
    if (n >= 0 && n < NUMSOUNDS && samples[n].chunk) {
        Mix_PlayChannel(-1, samples[n].chunk, 0);
    }
}
