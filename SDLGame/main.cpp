#include <SDL.h>
#include <stdio.h>
#include "GameObject.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int FPS = 60;
const int SCREEN_TICKS_PS = 1000 / FPS;

int main( int argc, char* args[] )
{
	//The window we'll be rendering to
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	bool quit = false;


	GameObject* spaceShip = new GameObject(SCREEN_WIDTH, SCREEN_HEIGHT, 200, 200);


	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
		return -1;
	}
	
	//Create window
	window = SDL_CreateWindow( "SDLGAME", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
	if( window == NULL )
	{
		printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
		return -1;
	}
	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_Event event;
	const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
	
	while (!quit) 
	{
		// FPS
		Uint32 current = SDL_GetTicks();
		Uint32 deltaTime = SDL_GetTicks() - current;
		
		if (deltaTime < SCREEN_TICKS_PS) 
		{
			SDL_Delay(SCREEN_TICKS_PS - deltaTime);
		}

		while (SDL_PollEvent(&event) != 0) 
		{
			if (event.type == SDL_QUIT) 
			{
				quit = true;
			}
			
		}
		
		if (currentKeyStates[SDL_SCANCODE_LEFT]) 
		{
			
			spaceShip->setDirection(-M_PI / 100);
		}

		if (currentKeyStates[SDL_SCANCODE_RIGHT])
		{
			spaceShip->setDirection(M_PI / 100);
		}		

		if (currentKeyStates[SDL_SCANCODE_UP])
		{
			// Here be dragons
			spaceShip->setMove();
			
		}
		if (currentKeyStates[SDL_SCANCODE_SPACE]) 
		{
			spaceShip->shootProjectile();
		}

		// Coloring on renderer RGBA
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(renderer);

		// Game stuff
		spaceShip->update();
		SDL_RenderGeometry(renderer, nullptr, spaceShip->getVertex(), 3, nullptr, 0);

		SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
		SDL_RenderDrawPoints(renderer, spaceShip->getProjectile(), 10);

		SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0xFF, 0x00);
		SDL_RenderDrawRects(renderer, spaceShip->getEnemy(), 10);

		SDL_RenderPresent(renderer);
	}	
	
	//Destroy window
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow( window );
	renderer = NULL;
	window = NULL;

	//Quit SDL subsystems
	SDL_Quit();

	return 0;
}


