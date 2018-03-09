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
#define SCROLL_SPEED 25
#define PLAYER_SPEED 5
#define NUM_POKEBALLS 16
#define NUM_POKEMONS 16
//#define MIN_POKEMONS 4
#define POKEBALLSPEED 3
#define POKEMONSPEED 2
#define READMESPEED 4
#define DETECTIONRADIUS 300
#define PLAYERSIZE 70
#define POKEBALLSIZE 25 //before 40
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
	bool readme; //if this projectile is a BONUS README
	int randomValue;
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
	SDL_Texture* gameOverTexture = nullptr;
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
	Mix_Chunk* fx_capture = nullptr;
	Mix_Chunk* fx_damage = nullptr;
	Mix_Chunk* fx_gameOver = nullptr;
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
	bool masterBall = false;
	
	bool gameOver = false;
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
	
	g.pokebulletsTexture = SDL_CreateTextureFromSurface(g.renderer, IMG_Load("assets/pokeBalls.png"));
	g.pokemonTexture = SDL_CreateTextureFromSurface(g.renderer, IMG_Load("assets/diamond-pearl.png"));
	g.readmeTexture = SDL_CreateTextureFromSurface(g.renderer, IMG_Load("assets/readme.png"));
	g.playerTexture = SDL_CreateTextureFromSurface(g.renderer, IMG_Load("assets/AshPidg.png"));
	g.gameOverTexture = SDL_CreateTextureFromSurface(g.renderer, IMG_Load("assets/gameOver.png"));
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
	g.fx_shoot = Mix_LoadWAV("assets/fx/fireFX.wav");
	g.fx_capture = Mix_LoadWAV("assets/fx/captureFX.wav");
	g.fx_damage = Mix_LoadWAV("assets/fx/damageFX.wav");
	g.fx_gameOver = Mix_LoadWAV("assets/fx/game_over.wav");

	//other general vars

	g.player_x = 100;
	g.player_y = windowHeight / 2;
	g.fire = g.up = g.down = g.left = g.right = false;
	//g.pokeSpriteSize = 91; //pixels for rect spritesheet cutter

	//pokemon spawn test / readme
	srand((int)SDL_GetTicks());

	//poketto = { rand() % windowWidth + 80, 120 + 80, 0, 0, true }; 
	//readme = { rand() % windowWidth + 120, 400, true };//rand() % windowHeight + 120, true};

	//g.respawnPokemons = true;

	//random posx between max screen size and offset sprite size,posy,spritesheetposx,spritesheetposy,alive
	
}

void finishGame()
{
	//if (g.gameOver == true)SDL_Delay(5000);
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
				case SDLK_LCTRL: g.fire = (event.key.repeat == 0); g.masterBall = true; break;
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
	if (g.gameOver == true)ret = false;
	return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
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
		Mix_PlayChannel(-1, g.fx_shoot, 0);
		g.fire = false;

		if (g.last_shot == NUM_POKEBALLS)
		{
			g.last_shot = 0;
			//g.pokebullets[i]
			for (int i = 0; i < NUM_POKEBALLS; ++i) //force new balls clean of the flag
			{
				g.pokebullets[i].masterBall = false;
			}
		}
			

		g.pokebullets[g.last_shot].alive = true;
		//g.pokebullets[g.last_shot].spritePosx = rand() % 2 + 1;
		//g.pokebullets[g.last_shot].spritePosy = rand() % 2 + 1;
		if (g.masterBall == false) // NORMAL SHOT
		{
			g.pokebullets[g.last_shot].sprite = 1;//rand() % (0 + 1);
			g.pokebullets[g.last_shot].x = g.player_x + POKEBALLSIZE*2;
			g.pokebullets[g.last_shot].y = g.player_y;
			++g.last_shot;
		}

		else if (g.masterBall == true) //set the flag FOR MASTERBALL SHOT
		{
			g.pokebullets[g.last_shot].x = g.player_x + POKEBALLSIZE;
			g.pokebullets[g.last_shot].y = g.player_y;
			g.pokebullets[g.last_shot].sprite = 0;
			g.masterBall = false;
			g.pokebullets[g.last_shot].masterBall = true; //
			++g.last_shot;

		}
		

		


	}

	for (int i = 0; i < NUM_POKEBALLS; ++i)
	{
		if (g.pokebullets[i].alive) //&& g.pokebullets[i].detected == false)  //while no detection
																		   // i dont care if the bullet is
																		   //special or not
		{
			if (g.pokebullets[i].x < windowWidth)
				g.pokebullets[i].x += POKEBALLSPEED;

			else
			{
				g.pokebullets[i].alive = false; //when te ball outscreen = alive false for later checks
				g.pokebullets[i].x = 0; //reset positions
				g.pokebullets[i].y = 0;
			}

			///////////////////////////////////////////////////////////////////////////////////////////
			//AUTOTARGET MASTERBALL missile
			//RADIAL DETECTION
			if (g.pokebullets[i].masterBall == true) //only check if the bullet is MASTERBALL
			{
				for (int j = 0; j < NUM_POKEMONS; j++)
				{
					//if (g.pokebullets[i].masterBall) //only routine detection if flag
					//{

					if (pow((g.pokebullets[i].x - g.pokemon[j].x), 2) +
						pow((g.pokebullets[i].y - g.pokemon[j].y), 2) <= pow(DETECTIONRADIUS, 2))
					{
						if (g.pokebullets[i].detected == false) //if not detected yet assign indexes
						{
							g.pokebullets[i].detected = true; //masterball detected a subject
							g.pokebullets[i].enemyTargetIndex = j;
							//g.player_x = 0;
						}

					}

				}
			}
		}
		else if (g.pokebullets[i].x > windowWidth - 200) //if the pokeball go out of screen limits, DIE//|| g.pokebullets[i].x < windowWidth)
			g.pokebullets[i].alive == false;



		//if enemy targeted//////////////////////////////////////

		if (g.pokebullets[i].detected )//&& g.pokebullets[i].masterBall) //workaround to go to X,Y position
		{
		////////////////////////////////////////////////////////////////////////////////////////
		//Try to implement by "complex" method, but the implemented work like a charm and dont need this

		//int squareT = (pow((poketto.x - g.player_x + 120) / (POKEBALLSPEED + POKEMONSPEED),2));
		//g.currentTime = SDL_GetTicks();
		//g.pokebullets[i].y = (1 / 2) * (2 * (poketto.y - g.pokebullets[i].y) / squareT * squareT); //(pow((poketto.x - g.player_x + 120) / (POKEBALLSPEED + POKEMONSPEED), 2)) * (pow(poketto.x - g.player_x + 120 / (POKEBALLSPEED + POKEMONSPEED), 2)));

		//////////////////////////////////////////////////////////////////////////////////////////

		g.dirx = g.pokemon[g.pokebullets[i].enemyTargetIndex].x - g.pokebullets[i].x;
		g.diry = g.pokemon[g.pokebullets[i].enemyTargetIndex].y - g.pokebullets[i].y;

		g.length = sqrt(g.dirx*g.dirx + g.diry*g.diry);

		g.dirx /= g.length; //normalize vector (make 1 unit length)
		g.diry /= g.length;
		g.dirx *= POKEBALLSPEED*2; //scale to our speed
		g.diry *= POKEBALLSPEED*2;
		g.pokebullets[i].x += g.dirx;
		g.pokebullets[i].y += g.diry;

		if (g.pokebullets[i].x +(POKEBALLSIZE/2) == g.pokemon[g.pokebullets[i].enemyTargetIndex].x +(POKEMONSIZE/2) ||
			g.pokebullets[i].y + (POKEBALLSIZE/2) == g.pokemon[g.pokebullets[i].enemyTargetIndex].y + (POKEBALLSIZE/2)) //if rendezvous = BOOM
		
			g.pokebullets[i].alive = false;
			g.pokebullets[i].detected = false;
			
			--g.pokemonsOnScreen;
			
			Mix_PlayChannel(-1, g.fx_capture, 0);
		
		}

	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	//SPAWN POKEMONS AND BONUSES
	

	if (g.respawnPokemons)
	{
		g.respawnPokemons = false;

		//if (g.lastSpawn == NUM_POKEMONS)
			//g.lastSpawn = 0;

		srand(SDL_GetTicks());
		int readmesQuantityForRound = rand() % 3 + 1; // if something crash, remove this and static value
		int spawnedReadmes = 0;

	 	for (int i = 0; i < NUM_POKEMONS; i++)
		{
			

			if (g.pokemon[i].alive == false)
			{

				g.pokemon[i] = { rand() % windowWidth + windowWidth + 100, rand() % windowHeight + 100 ,0,0 };//assign random coordinates with constraints
				g.pokemon[i].alive = true;

			}
				if (spawnedReadmes < readmesQuantityForRound)
				{
					g.pokemon[i].readme = true;
					srand(SDL_GetTicks());
					g.pokemon[i].randomValue = rand() % windowHeight;
 					++spawnedReadmes;
				}
				//else g.pokemon[i].readme = false;
				++g.pokemonsOnScreen;
			
				
		}
		
		
	}
	
	//proceed to move

	for (int i = 0; i < NUM_POKEMONS; ++i)
	{

		if (g.pokemon[i].alive)
		{
			if (g.pokemon[i].x > 0 - POKEMONSIZE)//////////////////////////////////////////////
			{
				g.pokemon[i].x -= POKEMONSPEED;
				if (g.pokemon[i].readme)
				{
					g.pokemon[i].x -= READMESPEED;
					srand(SDL_GetTicks());
					g.pokemon[i].y = g.pokemon[i].randomValue + (80 * cos(((2 * 3.1415) / 600)*(g.pokemon[i].x + 1.5f*SDL_GetTicks())));
				}
			}
			else 
			{

 			g.pokemon[i].alive = false;
			g.pokemonsOnScreen--;
			////////////////////////////////////////////////////////PROVISIONAL GAMEOVER/////////////
			if (g.pokemon[i].readme == true && g.pokemon[i].x  <= 0)g.gameOver = true;
			/////////////////////////////////////////////////////////////////////////////////////////
			}
		}

	}
	//g.minPokemonsOnScreen = rand() % 7 + 3;
	if (g.pokemonsOnScreen <= g.minPokemonsOnScreen) //minimum pokemons on screen before respawn more
	{
	 	srand(SDL_GetTicks());
    	g.minPokemonsOnScreen = rand() % 7 + 3; //WORKS ?
		g.respawnPokemons = true;

	}



	////////////////////////////////////////////////////////////////////////////////////////////////////////

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
			//if ((abs((g.player_x + (PLAYERSIZE / 2)) - (g.pokemon[i].x + (POKEMONSIZE / 2))) < POKEMONSIZE / 2 + POKEBALLSIZE / 2) && (abs(g.player_y + (PLAYERSIZE / 2) - g.pokemon[i].y + (POKEMONSIZE / 2)) < (PLAYERSIZE / 2 + POKEBALLSIZE / 2)))
			if (abs(g.pokemon[i].x - g.player_x +(PLAYERSIZE - 50) ) < POKEMONSIZE / 2 + POKEBALLSIZE / 2 && abs(g.player_y  - g.pokemon[i].y) < PLAYERSIZE / 2 + POKEBALLSIZE / 2)
			{
				//g.player_x = 0;
				//event here when player collides with pokemon
				Mix_PlayChannel(-1, g.fx_damage, 0);

			}
		}
	}

	///////////////////////////POKEBALLS COLLISIONS

	for (int i = 0; i < NUM_POKEBALLS; i++)
	{
		if (g.pokebullets[i].alive && g.pokebullets[i].detected == false) //no collision if the ball is MASTERBALL
		{
			for (int j = 0; j < NUM_POKEMONS; j++)
			{
				if (g.pokemon[j].alive)
				{
					if ((abs(g.pokemon[j].x - (g.pokebullets[i].x - POKEBALLSIZE * 2 - 25))) < POKEMONSIZE / 2 + POKEBALLSIZE / 2 && abs((g.pokebullets[i].y - (POKEBALLSIZE) - g.pokemon[j].y)) < PLAYERSIZE / 2 + POKEBALLSIZE / 2)
					{
						//g.player_x = 0;
						g.pokemon[j].alive = false;
						g.pokebullets[i].alive = false;
						
						g.pokemonsOnScreen--; //rest counter to draw
						Mix_PlayChannel(-1, g.fx_capture, 0);

					}
				}

			}
		}
	}




}

void Draw()
{
	/////////////////////////////////background////////////////////////////////////

	g.scroll += SCROLL_SPEED;
	if (g.scroll >= g.background_width)
		g.scroll = 0;

	g.target = { -g.scroll,0,g.background_width, windowHeight };
	SDL_RenderCopy(g.renderer, g.background, NULL, &g.target);
	g.target.x += g.background_width;
	SDL_RenderCopy(g.renderer, g.background, NULL, &g.target);

	

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
			g.spritesRect = { g.pokemon[i].spriteSheetPosX, g.pokemon[i].spriteSheetPosY ,80,80 }; //rect to cut off the sprites
			
			if (g.pokemon[i].readme == false)
			{
				g.target = { g.pokemon[i].x , g.pokemon[i].y , POKEMONSIZE, POKEMONSIZE }; //target rect to render
				SDL_RenderCopy(g.renderer, g.pokemonTexture, &g.spritesRect, &g.target);
			}
			else
			{
				g.target = { g.pokemon[i].x , g.pokemon[i].y , READMESIZE, READMESIZE }; //target rect to render
				SDL_RenderCopy(g.renderer, g.readmeTexture, NULL, &g.target);
			}
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

	///////////////////////////////////////PLAYER//////////////////////////////////////////////////
	g.target = { g.player_x, g.player_y, PLAYERSIZE, PLAYERSIZE };
	SDL_RenderCopy(g.renderer, g.playerTexture, NULL, &g.target);
	/////////////////////////////////////////////////////////////////////////////////////////////

	if (g.gameOver == true)
		 {
			 Mix_PlayChannel(1, g.fx_gameOver, 0);
			 int lastTime = SDL_GetTicks();
			 int currentime = 0;
			 Mix_FreeMusic(g.music);
			 g.music = nullptr;
			// Mix_HaltMusic();
			
			
			g.target = { 0,0,windowWidth,windowHeight };
			SDL_RenderCopy(g.renderer, g.gameOverTexture, nullptr, &g.target); //g.background has to be gameover texture
			SDL_RenderPresent(g.renderer); //workaround OUT OF TIME
			//SDL_Delay(5000); //workaround OUT OF TIME
			while (currentime - lastTime < 10000) currentime = SDL_GetTicks(); //WORKAROUND OUT OF TIME without delay

			//
		}
	
	
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