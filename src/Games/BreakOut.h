#include "Engine/Game.h"
#include "Engine/Scene.h"
#include "Engine/Entity.h"
#include "Engine/Components.h"
#include "Engine/Systems.h"
#include "Engine/RandomSystem.h"
#include <entt/entt.hpp>
#include <print>

struct SpriteComponent {
	int width;
	int height;
	SDL_Color color;
};

struct PlayerTagComponent {};
struct BallTagComponent {};

class PlayerSpawnSetupSystem : public SetupSystem {
	void run(){
		Entity* player = scene -> createEntity("PLAYER", 250, 350);
		player -> addComponent<VelocityComponent>(5, 5);
		player -> addComponent<SpriteComponent>(150, 25, SDL_Color{255, 0, 0});
		player -> addComponent<PlayerTagComponent>();
	}
};

class PlayerInput : public EventSystem {
	virtual void run(SDL_Event e){
		const Uint8* ks = SDL_GetKeyboardState(NULL);

		auto view = scene->r.view<PlayerTagComponent, VelocityComponent, PositionComponent>();

        auto player = view.front();
		auto pos = view.get<PositionComponent>(player);
        auto& vel = view.get<VelocityComponent>(player);

		if (ks[SDL_SCANCODE_A] && pos.x > 0) {
            vel.x = -200;
        } else if (ks[SDL_SCANCODE_D] && pos.x < 480) {
            vel.x = 200;
        } else {
            vel.x = 0;
        }
	}
};

class BallSpawnSetupSystem : public SetupSystem {
	void run(){
		Entity* ball = scene -> createEntity("BALL", 320, 150);
		ball -> addComponent<VelocityComponent>(-75, 150);
		ball -> addComponent<SpriteComponent>(30, 30, SDL_Color{255, 255, 255});
		ball -> addComponent<BallTagComponent>();
	}
};

class BlocksSpawnSetupSystem : public SetupSystem {
	public:
    	RandomSystem randomSystem;

	void run(){
		int cubeWidth = 60;
		int cubeHeight = 50;
		int numberOfCubes = 10;
		int spacing = (640 - cubeWidth * numberOfCubes) / (numberOfCubes + 1);
		
		for(int i = 0; i < 2; ++i){
			int yPos = i * cubeHeight;
			
			for(int j = 0; j < numberOfCubes; ++j){			
				int xPos = spacing + j * (cubeWidth + spacing);
				Entity* block = scene -> createEntity("BLOCK", xPos, yPos);

                std::array<int, 3> randomColorArray = randomSystem.getRandomIntArray();
                SDL_Color randomColor = {
                    static_cast<Uint8>(randomColorArray[0]),
                    static_cast<Uint8>(randomColorArray[1]),
                    static_cast<Uint8>(randomColorArray[2]),
                    255
                };
				
				block -> addComponent<SpriteComponent>(cubeWidth, cubeHeight, randomColor);
			}		
		}
	}
};

class MovementSystem : public UpdateSystem {
	void run(float dT) {
		auto view = scene->r.view<PositionComponent, VelocityComponent>();

		for (auto e : view) {
			auto& pos = view.get<PositionComponent>(e);
			auto vel = view.get<VelocityComponent>(e);

			pos.x += vel.x * dT;
			pos.y += vel.y * dT;
		}
	}
};

class WallHitSystem : public UpdateSystem {
	void run(float dT) {
		auto view = scene->r.view<BallTagComponent, PositionComponent, VelocityComponent, SpriteComponent>();

		auto ball = view.front();

		auto pos = view.get<PositionComponent>(ball);
		auto spr = view.get<SpriteComponent>(ball);
		auto& vel = view.get<VelocityComponent>(ball);
		
		int newPosX = pos.x + vel.x * dT;
		int newPosY = pos.y + vel.y * dT;

		if (newPosX < 0 || newPosX + spr.width > 640) {
			vel.x *= -1;
		}

		if (newPosY < 0 || newPosY + spr.height > 480) {
			vel.y *= -1;
		}	
	}
};

class SquareRenderSystem : public RenderSystem {
	void run(SDL_Renderer* renderer) {
		auto view = scene->r.view<PositionComponent, SpriteComponent>();
		for (auto e : view) {
			auto pos = view.get<PositionComponent>(e);
			auto spr = view.get<SpriteComponent>(e);

			SDL_SetRenderDrawColor(renderer, spr.color.r, spr.color.g, spr.color.b, spr.color.a);
			SDL_Rect r = { pos.x, pos.y, spr.width, spr.height };
			SDL_RenderFillRect(renderer, &r);
		}
	}
}; 

class BreakOut : public Game {
	public:
		Scene* sampleScene;
    	entt::registry r;

	public:
		BreakOut(): Game("BreakOut", 640, 480) {

		}

		void setup(){
			sampleScene = new Scene("BreakOut", r);

			addSetupSystem<PlayerSpawnSetupSystem>(sampleScene);
			addEventSystem<PlayerInput>(sampleScene);
			addSetupSystem<BallSpawnSetupSystem>(sampleScene);
			addSetupSystem<BlocksSpawnSetupSystem>(sampleScene);
			addUpdateSystem<MovementSystem>(sampleScene);
			addUpdateSystem<WallHitSystem>(sampleScene);
			addRenderSystem<SquareRenderSystem>(sampleScene);

			setScene(sampleScene);
		}
};