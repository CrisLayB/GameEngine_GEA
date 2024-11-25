#pragma once
#include "Engine/Game.h"
#include "DemoGame/Tilemap.h"
#include "Engine/Scene.h"
#include "Engine/Entity.h"
#include "Engine/Components.h"
#include "Engine/Systems.h"
#include "Engine/Graphics/TextureManager.h"
#include "Engine/Graphics/Texture.h"
#include "DemoGame/Collidres.h"
#include "DemoGame/Player.h"
#include "DemoGame/Sound.h"
#include <entt/entt.hpp>

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
    samurai->addComponent<BoxColliderComponent>(SDL_Rect{55, 20, 50, 120}, SDL_Color{255, 0, 0});
	}
};

class MongolSpawnSetupSystem : public SetupSystem {
  void run(){
    Entity* mongol = scene->createEntity("MONGOL", -300, 400);
    mongol->addComponent<EnemyComponent>();
    mongol->addComponent<VelocityComponent>(100, 0);
    mongol->addComponent<TextureComponent>("assets/Sprites/MongolComplete.png");
    mongol->addComponent<SpriteComponent>("assets/Sprites/MongolComplete.png", 160, 160, 1, 3, 1000);
    mongol->addComponent<BoxColliderComponent>(SDL_Rect{0, 20, 160, 130}, SDL_Color{255, 0, 0});
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
public:
  void run(SDL_Event event){
    auto view = scene -> r.view<PlayerComponent, VelocityComponent, PositionComponent>();
    for(auto e : view){
      auto& player = view.get<PlayerComponent>(e);
      auto& vel = view.get<VelocityComponent>(e);

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

        if (event.key.keysym.sym == SDLK_SPACE) {
          float currentTime = SDL_GetTicks() / 1000.0f;
          if (currentTime - player.lastAttackTime >= 0.5f) {
            std::cout << "Player attacks!" << std::endl;
            player.isAttack = true;
            player.lastAttackTime = currentTime;
            attackSound();
          }
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

private:
  void attackSound() {
    if (!game) {
        std::cerr << "Error: Game pointer is null." << std::endl;
        return;
    }

    if (!game->soundManager) {
        std::cerr << "Error: Sound manager is not initialized." << std::endl;
        return;
    }
    
    FMOD::System* soundManager = game->soundManager;
    
    auto soundView = scene->r.view<SoundComponent>();
    for (auto soundEntity : soundView) {
        auto& sound = soundView.get<SoundComponent>(soundEntity);

        if (sound.type == SoundType::EFFECT && sound.identifier == NameSound::ATTACK) {
            if (!sound.sound) {
                std::cerr << "Error: Sound object is null for ATTACK!" << std::endl;
                return;
            }

            if (sound.channel) {
                sound.channel->stop(); // Stop the previous sound if playing
            }

            // Play the sound
            FMOD_RESULT result = soundManager->playSound(sound.sound, nullptr, false, &sound.channel);
            if (result != FMOD_OK) {
                std::cerr << "Error playing sound: " << std::endl;
            }
            return;
        }
    }
  }
};

class BackgroundSetupSystem : public SetupSystem {
public:
  void run() override {
    Entity* background = scene->createEntity("BACKGROUND");
    const std::string& bgfile = "assets/Backgrounds/ScenarioComplete.png";
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

class PlayerAttackSystem : public UpdateSystem {
public:
  void run(float dT) override {
    auto playerView = scene->r.view<PlayerComponent, BoxColliderComponent, PositionComponent>();
    auto enemyView = scene->r.view<EnemyComponent, BoxColliderComponent, PositionComponent>();

    for (auto player : playerView) {
      auto& playerPos = playerView.get<PositionComponent>(player);
      auto& playerCollider = playerView.get<BoxColliderComponent>(player);
      auto& playerStruct = playerView.get<PlayerComponent>(player);

      SDL_Rect playerRect = {
        playerPos.x + playerCollider.rect.x,
        playerPos.y + playerCollider.rect.y,
        playerCollider.rect.w,
        playerCollider.rect.h
      };

      for (auto enemy : enemyView) {
        auto [enemyPos, enemyCollider] = enemyView.get<PositionComponent, BoxColliderComponent>(enemy);

        SDL_Rect enemyRect = {
          enemyPos.x + enemyCollider.rect.x,
          enemyPos.y + enemyCollider.rect.y,
          enemyCollider.rect.w,
          enemyCollider.rect.h
        };

        if (SDL_HasIntersection(&playerRect, &enemyRect) && playerStruct.isAttack) {
          auto& enemyStruct = enemyView.get<EnemyComponent>(enemy);
          enemyStruct.life--;

          std::cout << "Enemy hit! Enemy life: " << enemyStruct.life << std::endl;
          if (enemyStruct.life <= 0) {
            playerStruct.score++;
            enemyStruct.life = enemyStruct.originalLife;
            //scene->r.destroy(enemy); // Instead of this, call a method for Enemy can edit the position
            setEnemyPosition(scene->r, enemy, 400);
          }
        }
      }
      playerStruct.isAttack = false;
    }
  }
};

class SamuraisVsMongols_Level1 : public Game {
public:
	Scene* sampleScene;
	entt::registry r;

public:
	SamuraisVsMongols_Level1()
	: Game("Samurais Vs Mongols", 1130, 636)
	{}

	void setup(){
		sampleScene = new Scene("Samurais Vs Mongols - Level 1", r, renderer);

    // ==> Setup Systems
    addSetupSystem<TilemapSetupSystem>(sampleScene);
		addSetupSystem<SamuraiSpawnSetupSystem>(sampleScene);
    addSetupSystem<MongolSpawnSetupSystem>(sampleScene);
		addSetupSystem<BackgroundSetupSystem>(sampleScene);
		addSetupSystem<TextureSetupSystem>(sampleScene);
    addEventSystem<SamuraiMovementInputSystem>(sampleScene);

    // ==> Sound Systems
    addSetupSystem<SoundSetupSystem>(sampleScene);
    addSetupSystem<BackgroundMusicSystem>(sampleScene);

    // ==> Update Systems
    addUpdateSystem<ColliderResetSystem>(sampleScene);
    //addUpdateSystem<PlayerWallCollisionSystem>(sampleScene);

    // Detection Systems
    addUpdateSystem<PlayerIsInsideEnemyDetectionSystem>(sampleScene);

    // Collisions Systems Integraded
    addUpdateSystem<EnemyWillAttackPlayerCollisionSystem>(sampleScene);
    
    addUpdateSystem<PlayerAttackSystem>(sampleScene);

    // ==> Render Systems
    addRenderSystem<TilemapRenderSystem>(sampleScene);
    addUpdateSystem<MovementSystem>(sampleScene);
		addUpdateSystem<SpriteAnimationSystem>(sampleScene);
		addRenderSystem<BackgroundRenderSystem>(sampleScene);
		addRenderSystem<SpriteRenderSystem>(sampleScene);
    addRenderSystem<ColliderRenderSystem>(sampleScene);

		setScene(sampleScene);
	}
};