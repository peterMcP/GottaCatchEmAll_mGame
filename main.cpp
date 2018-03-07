//be aware
//a wild readme appeared
#include <stdlib.h>

#include "SDL/include/SDL.h"
#include "SDL_Image/include/SDL_image.h"
#include "SDL_Mixer/include/SDL_mixer.h"

#pragma comment(lib, "SDL/libx86/SDL2.lib")
#pragma comment(lib,"SDL/libx86/SDL2main.lib")
#pragma comment(lib,"SDL_Image/libx86/SDL2_image.lib")
#pragma comment(lib,"SDL_Mixer/libx86/SDL2_mixer.lib")

//globals
#define windowWidth 640
#define windowHeight 480
#define SCROLL_SPEED 3
#define PLAYER_SPEED 5
#define NUM_POKEBALLS 16
#define POKEBALLSPEED 7
#define POKEMONSPEED 9

struct projectile
{
	int x, y;
	//int spritePosx, spritePosy;
	int sprite; //store the position number of the texture inside the spritesheet
	bool alive;
};

struct pokemon
{
	int x, y;
	int spriteSheetPosX, spriteSheetPosY;
	bool alive;
}poketto;

struct readme
{
	int x, y;
	bool alive;
}readme;

struct globals
{
	int currentTime = SDL_GetTicks(); //store current time in ms, overflow at day 49 aprox of runtime
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Texture *background = nullptr;
	SDL_Texture* midground = nullptr;
	SDL_Texture* playerTexture = nullptr;
	SDL_Texture* pokebulletsTexture = nullptr;
	SDL_Texture* pokemonTexture = nullptr;
	SDL_Texture* readmeTexture = nullptr;
	SDL_Texture* gameOverScreen = nullptr;
	SDL_Texture* scoreScreen = nullptr;
	SDL_Rect target;
	SDL_Rect spritesRect;
	//int pokeSpritesMatrix[3][2] = { {0,0},{91,0},{0,91} };
	int pokeSpritesMatrix[3] = { 0, 91 , 182 };
	int pokeSpriteSize = 91;
	int background_width = 0;
	int player_x = 0;
	int player_y = 0;
	int last_shot = 0;
	bool fire, up, down, left, right;
	Mix_Music* music = nullptr;
	Mix_Chunk* fx_shoot = nullptr;
	int scroll = 0;
	int cloudScroll = 0;
	projectile pokebullets[16];
	pokemon pokemon[32];
	int maxPokemons = 10;
	int screenPokemons = 0;
}g; //create structure g at start the game

//////////////////////////////////////////////////////////////////////////////////////////////

void start()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	g.window = SDL_CreateWindow(
		"GottaCatchEmAll: A Wild Readme Appeared - v0.1",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		windowWidth,
		windowHeight,
		SDL_WINDOW_OPENGL);
	//if (g.window == NULL) { std::cout << "Could not create window: %s \n", SDL_GetError(); }

	g.renderer = SDL_CreateRenderer(g.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	IMG_Init(IMG_INIT_PNG);
	g.background = SDL_CreateTextureFromSurface(g.renderer, IMG_Load("assets/background.png"));
	g.playerTexture = SDL_CreateTextureFromSurface(g.renderer, IMG_Load("assets/player.png"));
	g.pokebulletsTexture = SDL_CreateTextureFromSurface(g.renderer, IMG_Load("assets/pokeBalls.png"));
	g.pokemonTexture = SDL_CreateTextureFromSurface(g.renderer, IMG_Load("assets/diamond-pearl.png"));
	g.readmeTexture = SDL_CreateTextureFromSurface(g.renderer, IMG_Load("assets/readme.png"));
	//SDL_QueryTexture a pointer filled in with the texture width in pixels
	SDL_QueryTexture(g.background, nullptr, nullptr, &g.background_width, nullptr); 

	//static spritesheet values of pokeballs
	

	if (g.background || g.playerTexture || g.pokebulletsTexture || g.pokemonTexture == NULL) {
		//load image error event here
		SDL_GetError();
		//handle here to exit when any texture not found
	}

	//g.target = { 0,0,640,480 }; //initializes rect target to 0

	Mix_Init(MIX_INIT_OGG);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	g.music = Mix_LoadMUS("assets/music1.ogg");
	//Mix_PlayMusic(g.music, -1);
	//Mix_VolumeMusic(32); //values from 0 to 128 = MIX_MAX_VOLUME
	//g.fx_shoot = Mix_LoadWAV("assets/name");

	//other general vars

	g.player_x = 100;
	g.player_y = windowHeight / 2;
	g.fire = g.up = g.down = g.left = g.right = false;
	//g.pokeSpriteSize = 91; //pixels for rect spritesheet cutter

	//pokemon spawn test / readme
	srand((int)SDL_GetTicks());

	poketto = { rand() % windowWidth + 80, rand() % windowHeight + 80, 0, 0, true }; 
	readme = { rand() % windowWidth + 120, rand() % windowHeight + 120, true};

	//random posx between max screen size and offset sprite size,posy,spritesheetposx,spritesheetposy,alive

}

void finishGame()
{
	Mix_FreeMusic(g.music);
	Mix_FreeChunk(g.fx_shoot);
	Mix_CloseAudio();
	Mix_Quit();
	SDL_DestroyTexture(g.scoreScreen);
	SDL_DestroyTexture(g.playerTexture);
	SDL_DestroyTexture(g.pokebulletsTexture);
	SDL_DestroyTexture(g.background);
	SDL_DestroyTexture(g.midground);
	SDL_DestroyTexture(g.pokemonTexture);
	SDL_DestroyTexture(g.readmeTexture);
	SDL_DestroyTexture(g.gameOverScreen);
	IMG_Quit();
	SDL_DestroyRenderer(g.renderer);
	SDL_DestroyWindow(g.window);
	SDL_Quit();
}

bool checkInput()
{
	bool ret = true;
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_WINDOWEVENT) {
			if (event.window.event == SDL_WINDOWEVENT_CLOSE) 
				ret = false;
		}

		if (event.type == SDL_KEYDOWN)
		{
			switch (event.key.keysym.sym)
			{
				case SDLK_ESCAPE: ret = false; break;
				case SDLK_UP: g.up = true; break;
				case SDLK_DOWN: g.down = true; break;
				case SDLK_RIGHT: g.right = true; break;
				case SDLK_LEFT: g.left = true; break;
				case SDLK_SPACE: g.fire = (event.key.repeat == 0);break;
				default: break;
			}
		}

		if (event.type == SDL_KEYUP)
		{
			switch (event.key.keysym.sym)
			{
				case SDLK_UP: g.up = false; break;
				case SDLK_DOWN: g.down = false; break;
				case SDLK_RIGHT: g.right = false; break;
				case SDLK_LEFT: g.left = false; break;
			}
		}
	}
	
	return ret;
}

void moveStuff()
{
	//up mov and collision
	if (g.up && g.player_y > 0) g.player_y -= PLAYER_SPEED;
	if (g.down && g.player_y < windowHeight - 160) g.player_y += PLAYER_SPEED;
	if (g.right && g.player_x < windowWidth - 160) g.player_x += PLAYER_SPEED;
	if (g.left && g.player_x > 0) g.player_x -= PLAYER_SPEED;

	if (g.fire)
	{
		g.fire = false;

		if (g.last_shot == NUM_POKEBALLS)
			g.last_shot = 0;

		g.pokebullets[g.last_shot].alive = true;
		//g.pokebullets[g.last_shot].spritePosx = rand() % 2 + 1;
		//g.pokebullets[g.last_shot].spritePosy = rand() % 2 + 1;
		g.pokebullets[g.last_shot].sprite = rand() % 3;
		g.pokebullets[g.last_shot].x = g.player_x + 100;
		g.pokebullets[g.last_shot].y = g.player_y;
		++g.last_shot;
	}
	
	for (int i = 0; i < NUM_POKEBALLS; ++i)
	{
		if (g.pokebullets[i].alive)
			g.pokebullets[i].x += POKEBALLSPEED;
	}

	if (poketto.alive)
	{
		poketto.x -= POKEMONSPEED;
		//HARMONIC WAVE for readme enemy
		poketto.y = 80 * cos(((2 * 3.1415) / 600)*(poketto.x + 1.5f*SDL_GetTicks()));
		
		if (poketto.x < 0 - 80) //for testing purposes
		{
			poketto.x = 640;
			poketto.y = 300;//rand() % windowHeight;
			poketto.spriteSheetPosX = 160;
			poketto.spriteSheetPosY = 0;
		}
		
	}

	if (readme.alive)
	{
		readme.x -= POKEMONSPEED;
		readme.y = 60 * cos(((1 * 3.1415) / 700)*(readme.x + 1.75f*SDL_GetTicks()));
		if (readme.x < 0 - 80)
		{
			readme.x = windowWidth;
		}
	}

}

void Draw()
{
	g.scroll += SCROLL_SPEED;
	if (g.scroll >= g.background_width)
		g.scroll = 0;

	g.target = { -g.scroll,0,g.background_width, windowHeight };
	SDL_RenderCopy(g.renderer, g.background, NULL, &g.target);
	g.target.x += g.background_width;
	SDL_RenderCopy(g.renderer, g.background, NULL, &g.target);

	g.target = { g.player_x, g.player_y, 110,110 };
	SDL_RenderCopy(g.renderer, g.playerTexture, NULL, &g.target);

	for (int i = 0; i < NUM_POKEBALLS; ++i)
	{


		if (g.pokebullets[i].alive) // 0, 91
		{
  			g.spritesRect = { g.pokeSpritesMatrix[g.pokebullets[i].sprite] ,
							  0,//g.pokeSpritesMatrix[g.pokebullets[i].sprite] , 
							  g.pokeSpriteSize, g.pokeSpriteSize };
			g.target = { g.pokebullets[i].x, g.pokebullets[i].y, 40, 40 };
			SDL_RenderCopy(g.renderer, g.pokebulletsTexture, &g.spritesRect, &g.target);
		}
	}

	if (poketto.alive)
	{
		g.spritesRect = { poketto.spriteSheetPosX, poketto.spriteSheetPosY ,80,80 };
		g.target = { poketto.x , poketto.y , 128, 128 };
		SDL_RenderCopy(g.renderer, g.pokemonTexture, &g.spritesRect, &g.target);
	}

	if (readme.alive)
	{
		g.target = { readme.x, readme.y, 60,60};
		SDL_RenderCopy(g.renderer, g.readmeTexture, NULL, &g.target);
	}
	
	
	SDL_RenderPresent(g.renderer);

}

int main(int argc, char* argv[])
{

	start();

	while (checkInput())
	{
		moveStuff();
		Draw();
	}

	finishGame();

	return 0;
}