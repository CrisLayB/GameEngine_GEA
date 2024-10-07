#pragma once
#include "Engine/Game.h"
#include "DemoGame/Tilemap.h"
#include "Engine/Scene.h"
#include "Engine/Entity.h"
#include "Engine/Components.h"
#include "Engine/Systems.h"
#include "Engine/Graphics/TextureManager.h"
#include "Engine/Graphics/Texture.h"
#include <entt/entt.hpp>
#include "DemoGame/Collidres.h"
#include "DemoGame/Player.h"

struct BackgroundComponent {
  std::string filename;
};


class SamuraiSpawnSetupSystem : public SetupSystem {
	void run(){
		Entity* samurai = scene->createEntity("SAMURAI", 500, 400);
		samurai->addComponent<PlayerComponent>();
		samurai->addComponent<VelocityComponent>(0, 0);
		samurai->addComponent<TextureComponent>("assets/Sprites/SamuraiComplete.png");
    samurai->addComponent<SpriteComponent>("assets/Sprites/SamuraiComplete.png", 160, 160, 1, 7, 1000);
    samurai->addComponent<BoxColliderComponent>(SDL_Rect{0, 0, 80, 80}, SDL_Color{255, 0, 0});

    Entity* shuriken = scene->createEntity("SHURIKEN", 500, 400);
		shuriken->addComponent<TextureComponent>("assets/Sprites/shuriken.png");
    shuriken->addComponent<SpriteComponent>("assets/Sprites/shuriken.png", 64, 64, 1, 6, 1000);
	}
};

class MongolSpawnSetupSystem : public SetupSystem {
  void run(){
    Entity* mongol = scene->createEntity("MONGOL", 50, 400);
    mongol->addComponent<EnemyComponent>();
    mongol->addComponent<VelocityComponent>(100, 0);
    mongol->addComponent<TextureComponent>("assets/Sprites/MongolComplete.png");
    mongol->addComponent<SpriteComponent>("assets/Sprites/MongolComplete.png", 160, 160, 1, 3, 1000);
    mongol->addComponent<BoxColliderComponent>(SDL_Rect{0, 0, 80, 80}, SDL_Color{255, 0, 0});
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

class SamuraiMovementInputSystem : public EventSystem {
  void run(SDL_Event event){
    auto view = scene -> r.view<PlayerComponent, VelocityComponent, PositionComponent>();
    for(auto e : view){
      auto& player = view.get<PlayerComponent>(e);
      auto& vel = view.get<VelocityComponent>(e);
      auto& pos = view.get<PositionComponent>(e);
      //auto& pos = view.get<PositionComponent>(e);

      //const int LEFT_LIMIT = -27;
      //const int RIGHT_LIMIT = 1015;
      //const int TOP_LIMIT = 340;
      //const int BOTTOM_LIMIT = 420;

      if(event.type == SDL_KEYDOWN){
        if(event.key.keysym.sym == SDLK_a){
          vel.x = -player.speed;          
        }
        if(event.key.keysym.sym == SDLK_d){
          vel.x = player.speed;
        }
        if(event.key.keysym.sym == SDLK_w){
          vel.y = -player.speed;
        }
        if(event.key.keysym.sym == SDLK_s){
          vel.y = player.speed;
        }

        // Lanza una shuriken al presionar 'X'
        if (event.key.keysym.sym == SDLK_x) {
          Entity* shuriken = scene->createEntity("SHURIKEN");
          shuriken->addComponent<ShurikenComponent>("assets/Sprites/shuriken.png");
          shuriken->addComponent<BoxColliderComponent>(SDL_Rect{0, 0, 80, 80}, SDL_Color{255, 0, 0});
          shuriken->addComponent<TextureComponent>("assets/Sprites/shuriken.png");
          shuriken->addComponent<SpriteComponent>("assets/Sprites/shuriken.png", 64, 64, 1, 6, 1000);
          shuriken->addComponent<VelocityComponent>(-300, 0);  // Velocidad hacia la derecha
          shuriken->addComponent<PositionComponent>(pos.x, pos.y);    // Posición inicial
        }

        
      }

      else if(event.type == SDL_KEYUP){
        if(event.key.keysym.sym == SDLK_a || event.key.keysym.sym == SDLK_d){
          vel.x = 0;
        }
        if(event.key.keysym.sym == SDLK_w || event.key.keysym.sym == SDLK_s){
          vel.y = 0;
        }
      }
    }
  }
};

class BackgroundSetupSystem : public SetupSystem {
public:
  void run() override {
    Entity* background = scene->createEntity("BACKGROUND");
    const std::string& bgfile = "assets/Backgrounds/background_level2.png";
    background->addComponent<TextureComponent>(bgfile);
    background->addComponent<BackgroundComponent>(bgfile);
  }
};


class BackgroundRenderSystem : public RenderSystem {
  void run(SDL_Renderer* renderer) {
    auto view = scene->r.view<BackgroundComponent>();
    for (auto e : view) {
      auto tex = view.get<BackgroundComponent>(e);
      auto texture = TextureManager::GetTexture(tex.filename);
      texture->render(renderer, 0, 0);
    }
  }
}; 



class SamuraisVsMongols : public Game {
public:
	Scene* sampleScene;
	entt::registry r;

public:
	SamuraisVsMongols()
	: Game("Samurais Vs Mongols", 1130, 636)
	{}

	void setup(){
		sampleScene = new Scene("Samurais Vs Mongols - Level 1", r, renderer);

    //addSetupSystem<TilemapSetupSystem>(sampleScene);
		addSetupSystem<SamuraiSpawnSetupSystem>(sampleScene);
    addSetupSystem<MongolSpawnSetupSystem>(sampleScene);
		addSetupSystem<BackgroundSetupSystem>(sampleScene);
		addSetupSystem<TextureSetupSystem>(sampleScene);

    // controls
    addEventSystem<SamuraiMovementInputSystem>(sampleScene);

    // collisions
    addUpdateSystem<ColliderResetSystem>(sampleScene);
    addUpdateSystem<PlayerWallCollisionSystem>(sampleScene);

    addUpdateSystem<PlayerPowerUpCollisionDetectionSystem>(sampleScene);
    addUpdateSystem<PlayerPowerUpCollisionSystem>(sampleScene);
    addUpdateSystem<PlayerEnemyCollisionDetectionSystem>(sampleScene);
    addUpdateSystem<ShurikenEnemyCollisionSystem>(sampleScene);

    addUpdateSystem<MovementSystem>(sampleScene);
		addUpdateSystem<SpriteAnimationSystem>(sampleScene);
		addRenderSystem<BackgroundRenderSystem>(sampleScene);
		addRenderSystem<SpriteRenderSystem>(sampleScene);
    addRenderSystem<ColliderRenderSystem>(sampleScene);
    //addRenderSystem<TilemapRenderSystem>(sampleScene);

		setScene(sampleScene);
	}
};