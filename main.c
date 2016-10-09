#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "SDL2/SDL.h"

typedef struct {
	SDL_Surface *sprite;
	SDL_Rect rect;
	int xVel;
	int yVel;
	int laserMax;
	int laserTimer;
	int laserCount;
	int laserVel;
} Player;

typedef struct {
	SDL_Surface *sprite;
	SDL_Rect rect;
} Laser;

typedef struct {
	SDL_Surface *sprite;
	SDL_Rect rect;
	int yVel;
} Background;

SDL_Window *window = NULL;
SDL_Surface *screen = NULL;
SDL_Event event;
const Uint8 *keystate;
Player *player;
Background *background;
Laser *playerLasers[50];
bool playing = true;

void init()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("Outer Fucking Space", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 400, 500, SDL_WINDOW_SHOWN);
	screen = SDL_GetWindowSurface(window);
}

SDL_Surface *loadSurface(char *image)
{
	SDL_Surface *optimizedSurface = NULL;
	SDL_Surface *loadedSurface = SDL_LoadBMP(image);
	SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0x00, 0x00, 0x00));
	
	optimizedSurface = SDL_ConvertSurface(loadedSurface, screen->format, 0);
	SDL_FreeSurface(loadedSurface);

	return optimizedSurface;
}

Player *loadPlayer(char *sprite, int x, int y)
{
	Player *player = malloc(sizeof(Player));

	player->sprite = loadSurface(sprite);
	player->rect.x = x;
	player->rect.y = y;
	player->xVel = 1;
	player->yVel = 1;
	player->laserTimer = 39;
	player->laserMax = 40;
	player->laserCount = 0;
	player->laserVel = 3;

	return player;
}

Laser *fireLaser(char *sprite, int x, int y) 
{
	Laser *laser = malloc(sizeof(Laser));
	
	laser->sprite = loadSurface(sprite);
	laser->rect.x = x;
	laser->rect.y = y;

	return laser;
}

Background *loadBackground(char *sprite)
{
	Background *background = malloc(sizeof(Background));

	background->sprite = loadSurface(sprite);
	background->rect.x = 0;
	background->rect.y = 0;
	background->yVel = 1;

	return background;
}

void killSDL() 
{
	SDL_FreeSurface(player->sprite);
	player = NULL;

	SDL_DestroyWindow(window);
	window = NULL;

	SDL_Quit();
}

void draw()
{
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));
	//SDL_BlitSurface(background->sprite, NULL, screen, &background->rect);
	SDL_BlitSurface(player->sprite, NULL, screen, &player->rect);

	int i;
	for(i = 0; i < player->laserCount; ++i) {
		SDL_BlitSurface(playerLasers[i]->sprite, NULL, screen, &playerLasers[i]->rect);
	}

	SDL_UpdateWindowSurface(window);
}

void updates()
{
	int i;
	for(i = 0; i < player->laserCount; ++i) {
		playerLasers[i]->rect.y -= player->laserVel;
	}

	for(i = 0; i < player->laserCount; ++i) {
		if(playerLasers[i]->rect.y <= 0) {
			SDL_FreeSurface(playerLasers[i]->sprite);
			playerLasers[i]->sprite = NULL;
		}
	}

	//background->rect.y += background->yVel;
}

int events() 
{
	while(SDL_PollEvent(&event) != 0) {
		if(event.type == SDL_QUIT) {
			playing = false;
		}
		
		if(event.type == SDL_KEYUP) {
			if(event.key.keysym.sym == SDLK_SPACE) {
				player->laserTimer = (player->laserMax - 1);
			}
		}
	}	

	keystate = SDL_GetKeyboardState(NULL);
	
	if(keystate[SDL_GetScancodeFromKey(SDLK_LEFT)] && player->rect.x >= 5) {
		player->rect.x -= player->xVel;		
	}

	if(keystate[SDL_GetScancodeFromKey(SDLK_RIGHT)] && player->rect.x <= 370) {
		player->rect.x += player->xVel;
	}

	if(keystate[SDL_GetScancodeFromKey(SDLK_UP)] && player->rect.y >= 5) {
		player->rect.y -= player->yVel;
	}

	if(keystate[SDL_GetScancodeFromKey(SDLK_DOWN)] && player->rect.y <= 465) {
		player->rect.y += player->yVel;
	}

	if(keystate[SDL_GetScancodeFromKey(SDLK_SPACE)]) {
		player->laserTimer += 1;
				
		if(player->laserTimer == player->laserMax) {
			playerLasers[player->laserCount++] = fireLaser("images/laser.bmp", (player->rect.x + 11), player->rect.y);
			player->laserTimer = 0;
		}
	}

	SDL_Delay(3);
	
	return 0;				
}

int main(int argc, char *argv[])
{
	init();
	player = loadPlayer("images/ship1.bmp", 190, 450);
	background = loadBackground("images/background1.bmp");

	while(playing) {
		events();
		updates();
		draw();
	}

	killSDL();

	return 0;
}
