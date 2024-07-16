#include <string>
#include <print>
#include <SDL2/SDL.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int MAX_FPS = 60;
const int BALL_SPEED = 100;
const int BALL_SIZE = 100;

//struct Ball {
//	int x, y, w, h;
//};

//Ball player1 = {1, 2, 3, 4};
//Ball player1;
//player1.x = 1;
//player1.x = 2;
//player1.x = 3;
//player1.x = 4;

struct Rect {
	SDL_Rect rect = {0, 0, 100, 100};
	int vx = 0;
	int vy = 0;
	SDL_Color color = {0xFF, 0xFF, 0xFF, 0xFF};
};

SDL_Color red = {0xFF, 0x00, 0x00, 0xFF};
SDL_Color blue = {0x00, 0x00, 0xFF, 0xFF};

Rect ball1 = {{0, 0, 100, 100}, 0, 0, red};
Rect ball2 = {{SCREEN_WIDTH - BALL_SIZE, 0, 100, 100}, -BALL_SPEED, BALL_SPEED, blue};

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

	ball1.vx = 0;
	ball1.vy = 0;

	if(ks[SDL_SCANCODE_W]){
		//ball1.vy = -BALL_SPEED;
		ball1.vy = -BALL_SPEED;
	}
	if(ks[SDL_SCANCODE_A]){
		ball1.vx = -BALL_SPEED;
		//ball1.vy = BALL_SPEED;
	}
	if(ks[SDL_SCANCODE_S]){
		//ball1.vx = BALL_SPEED;
		ball1.vy = BALL_SPEED;
	}
	if(ks[SDL_SCANCODE_D]){
		//ball1.vx = -BALL_SPEED;
		ball1.vx = BALL_SPEED;
	}
};

void update(float dT){
	if (ball1.rect.x < 0) {
		ball1.vx *= -1;
	}
	if (ball2.rect.x < 0) {
		ball2.vx *= -1;
	}
	if (ball1.rect.y < 0) {
		ball1.vy *= -1;
	}
	if (ball2.rect.y < 0) {
		ball2.vy *= -1;
	}
	if (ball1.rect.x + ball1.rect.w > SCREEN_WIDTH) {
		ball1.vx *= -1;
	}
	if (ball2.rect.x + ball2.rect.w > SCREEN_WIDTH) {
		ball2.vx *= -1;
	}
	if (ball1.rect.y + ball1.rect.h > SCREEN_HEIGHT) {
		ball1.vy *= -1;
	}
	if (ball2.rect.y + ball2.rect.h > SCREEN_HEIGHT) {
		ball2.vy *= -1;
	}

	if(checkColission(ball1.rect, ball2.rect)){
		std::swap(ball1.vx, ball2.vx);
		std::swap(ball1.vy, ball2.vy);
	}
	
	ball1.rect.x += ball1.vx * dT;
	ball1.rect.y += ball1.vy * dT;
	ball2.rect.x += ball2.vx * dT;
	ball2.rect.y += ball2.vy * dT;	
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

		renderRect(renderer, ball1);
		renderRect(renderer, ball2);

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

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}