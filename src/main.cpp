#include <string>
#include <print>
#include <SDL2/SDL.h>
#include <vector>
#include <ctime>
#include <unordered_set>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int MAX_FPS = 60;
const int BALL_SPEED = 200;
const int CUBE_SPEED = 500;
const int BALL_SIZE = 100;

struct Rect {
	SDL_Rect rect = {0, 0, 100, 100};
	int vx = 0;
	int vy = 0;
	SDL_Color color = {0xFF, 0xFF, 0xFF, 0xFF};
};

SDL_Color red = {0xFF, 0x00, 0x00, 0xFF};
SDL_Color blue = {0x00, 0x00, 0xFF, 0xFF};
SDL_Color white = {0xFF, 0xFF, 0xFF, 0xFF};

Rect player = {{(SCREEN_WIDTH - BALL_SIZE) / 2, (SCREEN_HEIGHT - BALL_SIZE + 350) / 2, 150, 25}, 0, 0, red};
Rect ball = {{(SCREEN_WIDTH - BALL_SIZE) / 2, (SCREEN_HEIGHT - BALL_SIZE) / 2, 30, 30}, -BALL_SPEED, BALL_SPEED, white};

std::vector<Rect> cubes;

SDL_Color getRandomColor() {
    // Seed the random number generator with the current time
    std::srand(std::time(nullptr));

    Uint8 red = std::rand() % 256;
    Uint8 green = std::rand() % 256;
    Uint8 blue = std::rand() % 256;

    SDL_Color color = {red, green, blue, 0xFF};

    return color;
}

void renderRect(SDL_Renderer* renderer, Rect& ball){
	SDL_SetRenderDrawColor(renderer, ball.color.r, ball.color.g, ball.color.b, ball.color.a);
	SDL_RenderFillRect(renderer, &ball.rect);
}

bool checkColission(const SDL_Rect& a, const SDL_Rect& b){
	return (
		a.x < b.x + b.w &&
		a.x + a.w > b.x &&
		a.y < b.y + b.h &&
		a.y + a.h > b.h
	);
}

void handleInput(SDL_Event&e){
	// resolve
	const Uint8* ks = SDL_GetKeyboardState(NULL);

	player.vx = 0;
	player.vy = 0;
	
	if(ks[SDL_SCANCODE_A]){
		player.vx = -CUBE_SPEED;
	}

	if(ks[SDL_SCANCODE_D]){
		player.vx = CUBE_SPEED;
	}
};

void initComponents(){
	cubes.push_back({{(SCREEN_WIDTH - BALL_SIZE - 500) / 2, 0, 75, 75}, 0, 0, getRandomColor()});
	cubes.push_back({{(SCREEN_WIDTH - BALL_SIZE - 300) / 2, 0, 75, 75}, 0, 0, getRandomColor()});
	cubes.push_back({{(SCREEN_WIDTH - BALL_SIZE - 100) / 2, 0, 75, 75}, 0, 0, getRandomColor()});
	cubes.push_back({{(SCREEN_WIDTH - BALL_SIZE + 100) / 2, 0, 75, 75}, 0, 0, getRandomColor()});
	cubes.push_back({{(SCREEN_WIDTH - BALL_SIZE + 300) / 2, 0, 75, 75}, 0, 0, getRandomColor()});
	cubes.push_back({{(SCREEN_WIDTH - BALL_SIZE + 500) / 2, 0, 75, 75}, 0, 0, getRandomColor()});
}

void update(float dT){
	if (player.rect.x < 0) {
		player.vx *= -1; // ! CHECK THIS
	}

	if (ball.rect.x < 0) {
		ball.vx *= -1;
	}

	if (player.rect.y < 0) {
		player.vy *= -1;
	}

	if (ball.rect.y < 0) {
		ball.vy *= -1;
	}

	if (player.rect.x + player.rect.w > SCREEN_WIDTH) {
		player.vx *= -1; // ! CHECK THIS
	}

	if (ball.rect.x + ball.rect.w > SCREEN_WIDTH) {
		ball.vx *= -1;
	}

	if (player.rect.y + player.rect.h > SCREEN_HEIGHT) {
		player.vy *= -1;
	}

	if (ball.rect.y + ball.rect.h > SCREEN_HEIGHT) {
		ball.vy *= -1;
	}

	if(checkColission(player.rect, ball.rect)){
		ball.vy *= -1;
	}

	// Collisions for the Cubes
	for (size_t i = 0; i < cubes.size(); ++i){
		Rect& cube = cubes[i];
		if(checkColission(ball.rect, cube.rect)){
			ball.vy *= -1;
			cubes.erase(cubes.begin() + i);
			--i;
		}
	}
	
	player.rect.x += player.vx * dT;
	player.rect.y += player.vy * dT;
	ball.rect.x += ball.vx * dT;
	ball.rect.y += ball.vy * dT;	
}

int main(int argc, char* args[]) {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow("a", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	// El renderer es para pintar.
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool quit = false;
    SDL_Event e;

	int x = 0;
	int y = 0;

	//unsigned int frameCount = 1; // Aumentar frame maximo
	Uint32 frameCount = 1; // Uint32 es un estandar en SDL2
	Uint32 frameStartTimestamp;
	Uint32 frameEndTimestamp;
	Uint32 lastFrameTime = SDL_GetTicks();
	Uint32 lastUpdateTime = 0;
	float frameDuration = (1.0 / MAX_FPS) * 1000.0;
	float actualFrameDuration;
	int FPS = MAX_FPS;

	// Start Components
	initComponents();

    while (!quit) {
		frameStartTimestamp = SDL_GetTicks(); // Cuantos ticks del pc a pasado despues de sdls
		
		// delta time
		Uint32 currentFrameTime = SDL_GetTicks();
		float dT = (currentFrameTime - lastFrameTime) / 1000.0;
		lastFrameTime = currentFrameTime;
		
		// Pool Events
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
			handleInput(e);
        }

		// update
		update(dT);

        // render		
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);

		renderRect(renderer, player);
		renderRect(renderer, ball);

		for (size_t i = 0; i < cubes.size(); ++i){
			Rect& cube = cubes[i];
			renderRect(renderer, cube);
		}

        SDL_RenderPresent(renderer);

		frameEndTimestamp = SDL_GetTicks();
		actualFrameDuration = frameEndTimestamp - frameStartTimestamp;

		if(actualFrameDuration < frameDuration){
			SDL_Delay(frameDuration - actualFrameDuration);
		}

		// fps calculation
		frameCount++;
		Uint32 currentTime = SDL_GetTicks();
		Uint32 elapsedTime = currentTime - lastUpdateTime;
		if(elapsedTime > 1000){			
			FPS = (float)frameCount / (elapsedTime / 1000.0);
			lastUpdateTime = currentTime;
			frameCount = 0;
		}

		SDL_SetWindowTitle(window, std::to_string(FPS).c_str());
    }

	cubes.clear();
	
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}