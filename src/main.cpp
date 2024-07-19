#include <string>
#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <ctime>
#include <cstdlib>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int MAX_FPS = 60;
const int BALL_SPEED = 200;
const int CUBE_SPEED = 300;
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
bool blockedL = false;
bool blockedR = false;

std::vector<Rect> cubes;

SDL_Color getRandomColor() {
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

bool checkCollision(const SDL_Rect& a, const SDL_Rect& b){
    return (
        a.x < b.x + b.w &&
        a.x + a.w > b.x &&
        a.y < b.y + b.h &&
        a.y + a.h > b.h
    );
}

void handleInput(SDL_Event& e){
    const Uint8* ks = SDL_GetKeyboardState(NULL);

    player.vx = 0;
    player.vy = 0;

    if (ks[SDL_SCANCODE_A] && player.rect.x > 0) {
        player.vx = -CUBE_SPEED;
    }

    if (ks[SDL_SCANCODE_D] && player.rect.x + player.rect.w < SCREEN_WIDTH) {
        player.vx = CUBE_SPEED;
    }
}

void initComponents(int numberOfCubes, int cubeWidth, int cubeHeight){
	int spacing = (SCREEN_WIDTH - cubeWidth * numberOfCubes) / (numberOfCubes + 1);
    for (int i = 0; i < numberOfCubes; ++i) {
        int xPos = spacing + i * (cubeWidth + spacing);
        cubes.push_back({{xPos, 0, cubeWidth, cubeHeight}, 0, 0, getRandomColor()});
    }
}

void update(float dT){    
	if (player.rect.x + player.vx * dT < 0) {
        player.rect.x = 0;
    } else if (player.rect.x + player.rect.w + player.vx * dT > SCREEN_WIDTH) {
        player.rect.x = SCREEN_WIDTH - player.rect.w;
    } else {
        player.rect.x += player.vx * dT;
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

    if (ball.rect.x + ball.rect.w > SCREEN_WIDTH) {
        ball.vx *= -1;
    }

    if (ball.rect.y + ball.rect.h > SCREEN_HEIGHT) {
        ball.vy *= 0; // Game Over Velocity
    }

    if (checkCollision(player.rect, ball.rect)) {
        ball.vy *= -1;
    }

    // Collisions for the Cubes
    for (std::size_t i = 0; i < cubes.size(); ++i){
        Rect& cube = cubes[i];		
        if (checkCollision(ball.rect, cube.rect)) {
            if (ball.vy > 0) {
                ball.vy -= 75.0f;
                ball.vy *= -1;
            }

            if (ball.vy < 0) {
                ball.vy -= 75.0f;
                ball.vy *= -1;
            }

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
    std::srand(std::time(nullptr));

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow("a", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool quit = false;
    SDL_Event e;

    int x = 0;
    int y = 0;

    Uint32 frameCount = 1;
    Uint32 frameStartTimestamp;
    Uint32 frameEndTimestamp;
    Uint32 lastFrameTime = SDL_GetTicks();
    Uint32 lastUpdateTime = 0;
    float frameDuration = (1.0 / MAX_FPS) * 1000.0;
    float actualFrameDuration;
    int FPS = MAX_FPS;

    // Start Components
    int numberOfCubes = 10; 
    int cubeWidth = 60;
    int cubeHeight = 50;

    // Start Components
    initComponents(numberOfCubes, cubeWidth, cubeHeight);

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

        // Game Rules
        if (ball.vy == 0) {
            quit = true;
            std::cout << "GAME OVER" << std::endl;
            SDL_Delay(1000); // 1 second for stop and close the game
        }

        if (cubes.size() == 0) {
            quit = true;
            std::cout << "YOU WIN, come back to play again" << std::endl;
            SDL_Delay(1000); // 1 second for stop and close the game
        }

        // render
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);

        renderRect(renderer, player);
        renderRect(renderer, ball);

        for (std::size_t i = 0; i < cubes.size(); ++i){
            Rect& cube = cubes[i];
            renderRect(renderer, cube);
        }

        SDL_RenderPresent(renderer);

        frameEndTimestamp = SDL_GetTicks();
        actualFrameDuration = frameEndTimestamp - frameStartTimestamp;

        if (actualFrameDuration < frameDuration) {
            SDL_Delay(frameDuration - actualFrameDuration);
        }

        // fps calculation
        frameCount++;
        Uint32 currentTime = SDL_GetTicks();
        Uint32 elapsedTime = currentTime - lastUpdateTime;
        if (elapsedTime > 1000) {
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
