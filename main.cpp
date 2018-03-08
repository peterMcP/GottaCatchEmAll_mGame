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
#define NUM_POKEMONS 10
#define MIN_POKEMONS 4
#define POKEBALLSPEED 3
#define POKEMONSPEED 2
#define DETECTIONRADIUS 300
#define PLAYERSIZE 70
#define POKEBALLSIZE 40
#define POKEMONSIZE 110
#define READMESIZE 50

struct projectile
{
	int x, y;
	//int spritePosx, spritePosy;
	int sprite; //store the position number of the texture inside the spritesheet
	bool alive;
	bool masterBall; //if this projectile is a special weapon
	int enemyTargetIndex; //stores the index of the enemy targeted
	bool detected;
};

struct pokemon
{
	int x, y;
	int spriteSheetPosX, spriteSheetPosY;
	bool alive;
};

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
	int maxPokemons = 32;
	int pokemonsOnScreen = 0;
	int minPokemonsOnScreen = 3;
	int lastSpawn = 0;
	bool respawnPokemons = false;
	float dirx = 0;
	float diry = 0;
	int length = 0;
	
	//
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
	Mix_PlayMusic(g.music, -1);
	Mix_VolumeMusic(30); //values from 0 to 128 = MIX_MAX_VOLUME
	//g.fx_shoot = Mix_LoadWAV("assets/name");

	//other general vars

	g.player_x = 100;
	g.player_y = windowHeight / 2;
	g.fire = g.up = g.down = g.left = g.right = false;
	//g.pokeSpriteSize = 91; //pixels for rect spritesheet cutter

	//pokemon spawn test / readme
	srand((int)SDL_GetTicks());

	//poketto = { rand() % windowWidth + 80, 120 + 80, 0, 0, true }; 
	readme = { rand() % windowWidth + 120, 400, true };//rand() % windowHeight + 120, true};

	g.respawnPokemons = true;

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
	if (g.down && g.player_y < windowHeight - PLAYERSIZE) g.player_y += PLAYER_SPEED;
	if (g.right && g.player_x < windowWidth - PLAYERSIZE) g.player_x += PLAYER_SPEED;
	if (g.left && g.player_x > 0) g.player_x -= PLAYER_SPEED;


	//if fire true

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
		if (g.pokebullets[i].alive && g.pokebullets[i].detected == false)  //while no detection
																		   // i dont care if the bullet is
																		   //special or not
		{
			if (g.pokebullets[i].x < windowWidth)
				g.pokebullets[i].x += POKEBALLSPEED;

			else
				g.pokebullets[i].alive = false; //when te ball outscreen = alive false for later checks
			//autoTargeted missile
			//radial detection

			/*if (pow((g.pokebullets[i].x - poketto.x), 2) +
				pow((g.pokebullets[i].y - poketto.y), 2) <= pow(DETECTIONRADIUS, 2))
			{
				g.pokebullets[i].detected = true; //masterball missile index
				//pokemon index*/

				//}
		}
		else if (g.pokebullets[i].x > windowWidth - 200) //|| g.pokebullets[i].x < windowWidth)
			g.pokebullets[i].alive == false;

		//if enemy targeted

		/*if (g.pokebullets[i].detected) //workaround to go to X,Y position
		{
			//int squareT = (pow((poketto.x - g.player_x + 120) / (POKEBALLSPEED + POKEMONSPEED),2));
			//g.currentTime = SDL_GetTicks();
			//g.pokebullets[i].y = (1 / 2) * (2 * (poketto.y - g.pokebullets[i].y) / squareT * squareT); //(pow((poketto.x - g.player_x + 120) / (POKEBALLSPEED + POKEMONSPEED), 2)) * (pow(poketto.x - g.player_x + 120 / (POKEBALLSPEED + POKEMONSPEED), 2)));

			g.dirx = poketto.x - g.pokebullets[i].x;
			g.diry = poketto.y - g.pokebullets[i].y;

			g.length = sqrt(g.dirx*g.dirx + g.diry*g.diry);

			g.dirx /= g.length; //normalize vector (make 1 unit length)
			g.diry /= g.length;
			g.dirx *= POKEBALLSPEED*2; //scale to our speed
			g.diry *= POKEBALLSPEED*2;
			g.pokebullets[i].x += g.dirx;
			g.pokebullets[i].y += g.diry;

			if (g.pokebullets[i].x == poketto.x || g.pokebullets[i].y == poketto.y) //if rendezvous = BOOM
				g.pokebullets[i].alive = false;
				g.pokebullets[i].detected = false;

		}*/
	}

	//SPAWN POKEMONS
	//if (g.pokemonsOnScreen == 0) g.respawnPokemons = true;

	if (g.respawnPokemons)
	{
		g.respawnPokemons = false;

		//if (g.lastSpawn == NUM_POKEMONS)
			//g.lastSpawn = 0;

		for (int i = 0; i < NUM_POKEMONS; i++)
		{

			g.pokemon[i] = { rand() % windowWidth + windowWidth + 100, rand() % windowHeight + 100 ,0,0 };//assign random coordinates with constraints
			g.pokemon[i].alive = true;
			g.pokemonsOnScreen++;

		}
		
	}
	

	//proceed to move

	for (int i = 0; i < NUM_POKEMONS; ++i)
	{

		if (g.pokemon[i].alive)
		{
			if (g.pokemon[i].x > 0)
				g.pokemon[i].x -= POKEMONSPEED;
			else 
			{

			g.pokemon[i].alive = false;
			g.pokemonsOnScreen--;

			}
		}
	}

	if (g.pokemonsOnScreen < MIN_POKEMONS) g.respawnPokemons = true;




	/*if (poketto.alive)
	{
		poketto.x -= POKEMONSPEED;
		//HARMONIC WAVE for readme enemy

		//poketto.y = 80 * cos(((2 * 3.1415) / 600)*(poketto.x + 1.5f*SDL_GetTicks()));

		if (poketto.x < 0 - 80) //for testing purposes
		{
			poketto.x = 640;
			//poketto.y = 300;//rand() % windowHeight;
			poketto.spriteSheetPosX = 160;
			poketto.spriteSheetPosY = 0;
		}

	}*/

	////////////////////////README HARMONIC WAVE////////////////////////////
	if (readme.alive)
	{
		//int currentTime = SDL_GetTicks();

		readme.x -= POKEMONSPEED;
		readme.y = 200 + (80 * cos(((2 * 3.1415) / 900)*(readme.x + 2 * SDL_GetTicks())));
		if (readme.x < 0 - 80)
		{
			readme.x = windowWidth;
			//srand(SDL_GetTicks());
			//rand() % windowHeight;
		}
	}

	///////////////////////////////PLAYER COLLISIONS////////////////////////////
	int ix, iy = 0; //increment
	//ax = (g.player_x)
	for (int i = 0; i < NUM_POKEMONS; i++)
	{
		if (g.pokemon[i].alive)
		{
			if (abs(g.player_x - g.pokemon[i].x) < POKEMONSIZE / 2 + POKEBALLSIZE / 2 && abs(g.player_y - g.pokemon[i].y) < PLAYERSIZE / 2 + POKEBALLSIZE / 2)
			{
				//g.player_x = 0;
			}
		}
	}

	///////////////////////////POKEBALLS COLLISIONS

	for (int i = 0; i < NUM_POKEBALLS; i++)
	{
		if (g.pokebullets[i].alive)
		{
			for (int j = 0; j < NUM_POKEMONS; j++)
			{
				if (g.pokemon[j].alive)
				{
					if ((abs(g.pokebullets[i].x - g.pokemon[j].x)) < POKEMONSIZE / 2 + POKEBALLSIZE / 2 && abs((g.pokebullets[i].y - g.pokemon[j].y)) < PLAYERSIZE / 2 + POKEBALLSIZE / 2)
					{
						//g.player_x = 0;
						g.pokemon[j].alive = false;
						g.pokebullets[i].alive = false;
						g.pokemonsOnScreen--; //rest counter to draw

					}
				}

			}
		}
	}




}

void Draw()
{
	/////////////////////////////////player mov////////////////////////////////////

	g.scroll += SCROLL_SPEED;
	if (g.scroll >= g.background_width)
		g.scroll = 0;

	g.target = { -g.scroll,0,g.background_width, windowHeight };
	SDL_RenderCopy(g.renderer, g.background, NULL, &g.target);
	g.target.x += g.background_width;
	SDL_RenderCopy(g.renderer, g.background, NULL, &g.target);

	g.target = { g.player_x, g.player_y, PLAYERSIZE, PLAYERSIZE };
	SDL_RenderCopy(g.renderer, g.playerTexture, NULL, &g.target);

	/////////////////////////////////POKEBALLS/////////////////////////////////

	for (int i = 0; i < NUM_POKEBALLS; ++i)
	{


		if (g.pokebullets[i].alive) // 0, 91
		{
  			g.spritesRect = { g.pokeSpritesMatrix[g.pokebullets[i].sprite] ,
							  0,//g.pokeSpritesMatrix[g.pokebullets[i].sprite] , 
							  g.pokeSpriteSize, g.pokeSpriteSize };
			g.target = { g.pokebullets[i].x, g.pokebullets[i].y, POKEBALLSIZE, POKEBALLSIZE };
			SDL_RenderCopy(g.renderer, g.pokebulletsTexture, &g.spritesRect, &g.target);
		}
	}

	//////////////////////////////////POKEMONS/////////////////////////////////

	//g.pokemonsOnScreen = 0;

	for (int i = 0; i < NUM_POKEMONS; ++i)
	{
		//for (int j = 0; j < NUM_POKEMONS; j++)
			//if (g.pokemon[j].alive == false) g.pokemonsOnScreen++;
		//if (g.pokemonsOnScreen == g.minPokemonsOnScreen) g.pokemon[i].alive = true;
		if (g.pokemon[i].alive)
		{
			g.spritesRect = { g.pokemon[i].spriteSheetPosX, g.pokemon[i].spriteSheetPosY ,80,80 };
			g.target = { g.pokemon[i].x , g.pokemon[i].y , POKEMONSIZE, POKEMONSIZE };
			SDL_RenderCopy(g.renderer, g.pokemonTexture, &g.spritesRect, &g.target);
		}
	}

	/*if (poketto.alive)
	{
		g.spritesRect = { poketto.spriteSheetPosX, poketto.spriteSheetPosY ,80,80 };
		g.target = { poketto.x , poketto.y , 128, 128 };
		SDL_RenderCopy(g.renderer, g.pokemonTexture, &g.spritesRect, &g.target);
	}

	if (readme.alive)
	{
		g.target = { readme.x, readme.y, 60,60};
		SDL_RenderCopy(g.renderer, g.readmeTexture, NULL, &g.target);
	}*/
	
	
	SDL_RenderPresent(g.renderer);

}

int main(int argc, char* argv[])
{

	start();
	//g.respawnPokemons = true;

	while (checkInput())
	{
		moveStuff();
		Draw();
	}

	finishGame();

	return 0;
}