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

typedef struct {
	SDL_Surface *sprite;
	SDL_Rect rect;
	int xVel;
	int yVel;
} Grunt;

typedef struct {
	bool playing;
	int gruntCount;
	int gruntTimer;
	int gruntMax;
	Grunt *grunts[50];
	Laser *playerLasers[50];
} Game;

SDL_Window *window = NULL;
SDL_Surface *screen = NULL;
SDL_Event event;
const Uint8 *keystate;
Game *game;
Player *player;
Grunt *grunt;
Background *background;

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

Game *setupGame()
{
	Game *game = malloc(sizeof(Game));

	game->playing = true;
	game->gruntCount = 0;
	game->gruntTimer = 0;
	game->gruntMax = 200;
	game->grunts[50] = NULL;
	game->playerLasers[50] = NULL;

	return game;
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

Grunt *loadGrunt(char *sprite, int x, int y)
{
	Grunt *grunt = malloc(sizeof(Grunt));

	grunt->sprite = loadSurface(sprite);
	grunt->rect.x = x;
	grunt->rect.y = y;

	return grunt;
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
	for(i = 0; i < game->gruntCount; ++i) {
		SDL_BlitSurface(game->grunts[i]->sprite, NULL, screen, &game->grunts[i]->rect);
	}		

	for(i = 0; i < player->laserCount; ++i) {
		SDL_BlitSurface(game->playerLasers[i]->sprite, NULL, screen, &game->playerLasers[i]->rect);
	}

	SDL_UpdateWindowSurface(window);
}

void spawnGrunts(char *formation)
{
	game->gruntTimer += 1;

	if(strcmp(formation, "staggered") == 0) {
		if(game->gruntTimer == game->gruntMax) {
			int x = rand() % 400;
			game->grunts[game->gruntCount++] = loadGrunt("images/grunt.bmp", x, 0);
			game->gruntTimer = 0;
		}
	}
	else if(strcmp(formation, "straight") == 0) {
		if(game->gruntTimer == game->gruntMax) {
			int y = 40;
			int t;
			for(t = 0; t <= 6; t++) {
				game->grunts[game->gruntCount++] = loadGrunt("images/grunt.bmp", y, 0);
				y += 50;
			}			
			game->gruntTimer = 0;
		}
	}			

	int i;
	for(i = 0; i < game->gruntCount; ++i) {
		game->grunts[i]->rect.y += 1;

		if(game->grunts[i]->rect.y >= 500) {
			SDL_FreeSurface(game->grunts[i]->sprite);
			game->grunts[i]->sprite = NULL;
			free(game->grunts[i]->sprite);
			free(game->grunts[i]);
		}
	}
}

void shootLasers()
{
	int i;
	for(i = 0; i < player->laserCount; ++i) {
		game->playerLasers[i]->rect.y -= player->laserVel;

		if(game->playerLasers[i]->rect.y <= 0) {
			SDL_FreeSurface(game->playerLasers[i]->sprite);
			game->playerLasers[i]->sprite = NULL;
		}
	}
}	

void updates()
{
	shootLasers();
	//spawnGrunts("staggered");
	spawnGrunts("straight");
}

int events() 
{
	while(SDL_PollEvent(&event) != 0) {
		if(event.type == SDL_QUIT) {
			game->playing = false;
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
			game->playerLasers[player->laserCount++] = fireLaser("images/laser.bmp", (player->rect.x + 11), player->rect.y);
			player->laserTimer = 0;
		}
	}

	SDL_Delay(3);
	
	return 0;				
}

int main(int argc, char *argv[])
{	
	init();
	game = setupGame();
	player = loadPlayer("images/ship1.bmp", 190, 450);
	background = loadBackground("images/background1.bmp");

	while(game->playing) {
		events();
		updates();
		draw();
	}

	killSDL();

	return 0;
}
