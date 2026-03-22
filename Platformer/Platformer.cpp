#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include "Platformer.h"
#include "animation.h"
#include <vector>

using namespace std;

struct SDLState
{
	SDL_Window* window;
	SDL_Renderer* renderer;
	int width, height, logW, logH;
};

void cleanup(SDLState &state);
bool initialize(SDLState& state);

struct Resources
{
	const int ANIM_PLAYER_IDLE = 0;
	std::vector<Animation> playerAnims;

	std::vector<SDL_Texture *> textures;
	SDL_Texture* texIdle;

	SDL_Texture* loadTexture(SDL_Renderer *renderer,const std::string& filepath)
	{
		SDL_Texture* tex = IMG_LoadTexture(renderer, filepath.c_str());
		SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
		textures.push_back(tex);
		return tex;
	}
	void load(SDLState& state)
	{
		playerAnims.resize(6);
		playerAnims[ANIM_PLAYER_IDLE] = Animation(3, 1.0f);
		texIdle = loadTexture(state.renderer, "data/idle.png");
	}

	void unload()
	{
		for (SDL_Texture* tex : textures)
		{
			SDL_DestroyTexture(tex);
		}
	}
};

int main(int argc, char *argv[])
{	
	SDLState state;
	state.width = 1600;
	state.height = 900;
	state.logH = 320;
	state.logW = 640;

	if (!initialize(state))
	{
		
		return 1;
	}
		
	//load game assets
	Resources res;
	res.load(state);


	//setup game data
	const bool* keys = SDL_GetKeyboardState(nullptr);
	float playerX = 0;
	const float floor = state.logH;
	uint64_t prevTime = SDL_GetTicks();
	bool flipHorizontal = false;

	//start the game loop
	bool running = true; //loop condition prevents cleanup()
	while (running) //game loop
	{
		uint64_t nowTime = SDL_GetTicks();
		float deltaTime = (nowTime - prevTime) / 1000.0f;
		res.playerAnims[res.ANIM_PLAYER_IDLE].step(deltaTime);
		SDL_Event event{ 0 };
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_EVENT_QUIT:
				{
					running = false;
					break;
				}
				case SDL_EVENT_WINDOW_RESIZED:
				{
					state.width = event.window.data1;
					state.height = event.window.data2;
					break;
				}
			}
		}

		//handle movement
		float moveAmount = 0;
		if (keys[SDL_SCANCODE_A])
		{
			moveAmount += -95.0f;
			flipHorizontal = true;
		}
		else if (keys[SDL_SCANCODE_D])
		{
			moveAmount += 95.0f;
			flipHorizontal = false;
		}
		playerX += moveAmount * deltaTime;

		// perform drawing/rendering commands
		SDL_SetRenderDrawColor(state.renderer, 100, 149, 237, 255); // cornflower blue
		SDL_RenderClear(state.renderer);

		const float spriteSize = 32;
		SDL_FRect src{
			.x = res.playerAnims[res.ANIM_PLAYER_IDLE].currentFrame() * spriteSize,
			.y = 0,
			.w = spriteSize,
			.h = spriteSize
		};

		SDL_FRect dst{
			.x = playerX,
			.y = floor - spriteSize,
			.w = spriteSize,
			.h = spriteSize
		};

		
		
		SDL_RenderTextureRotated(state.renderer, res.texIdle, &src, &dst, 0, nullptr, (flipHorizontal) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);

		// swap buffers and present
		SDL_RenderPresent(state.renderer);
		prevTime = nowTime;
	}

	res.unload();
	cleanup(state);
	return 0;
}

bool initialize(SDLState& state)
{
	bool initSuccess = true;

	if (!SDL_Init(SDL_INIT_VIDEO)) //SDL init returns true if successful
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error initializing SDL3", nullptr);
		initSuccess = false;
	}

	//create the window with desired arguments
	state.window = SDL_CreateWindow("Platformer", state.width, state.height, SDL_WINDOW_RESIZABLE);
	if (!state.window)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error creating window.", state.window);
		cleanup(state);
		initSuccess = false;
	}

	//create the renderer
	state.renderer = SDL_CreateRenderer(state.window, nullptr);
	if (!state.renderer)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error initializing renderer", state.window);
		cleanup(state);
		initSuccess = false;
	}

	//configure presentation
	SDL_SetRenderLogicalPresentation(state.renderer, state.logW, state.logH, SDL_LOGICAL_PRESENTATION_LETTERBOX);

	return initSuccess;
}

void cleanup(SDLState &state)
{
	SDL_DestroyRenderer(state.renderer);
	SDL_DestroyWindow(state.window);
	SDL_Quit();
}
