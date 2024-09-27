#include "DemoGame/Tilemap.h"
#include "Engine/Game.h"
#include "Engine/Scene.h"
#include "Engine/Entity.h"
#include "Engine/Components.h"
#include "Engine/Systems.h"
<<<<<<< HEAD
#include "Engine/Graphics/TextureManager.h"
#include "Engine/Graphics/Texture.h"
=======
#include <SDL_events.h>
#include <SDL_timer.h>
#include <print>
>>>>>>> origin/backgrounds
#include <entt/entt.hpp>
#include "Sprites.h"
#include "Backgrounds.h"

struct PlayerComponent {

};

class SquareSpawnSetupSystem : public SetupSystem {
  void run() {
    Entity* square = scene->createEntity("SQUARE", 10, 10); 
    square->addComponent<PlayerComponent>();
    square->addComponent<VelocityComponent>(300);
    square->addComponent<TextureComponent>("assets/Sprites/cat.png");
    square->addComponent<SpriteComponent>("assets/Sprites/cat.png", 8, 8, 10, 8, 1000);
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

class MovementInputSystem : public EventSystem {
  void run(SDL_Event event) {
   auto view = scene->r.view<PlayerComponent, VelocityComponent>();
    for (auto e : view) {
      auto& vel = view.get<VelocityComponent>(e);
      
      if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_LEFT) {
          vel.x = -vel.speed;
        }
        if (event.key.keysym.sym == SDLK_RIGHT) {
          vel.x = vel.speed;
        }
        if (event.key.keysym.sym == SDLK_UP) {
          vel.y = -vel.speed;
        }
        if (event.key.keysym.sym == SDLK_DOWN) {
          vel.y = vel.speed;
        }
      } else if (event.type == SDL_KEYUP) {
        if (event.key.keysym.sym == SDLK_LEFT) {
          vel.x = 0;
        }
        if (event.key.keysym.sym == SDLK_RIGHT) {
          vel.x = 0;
        }
        if (event.key.keysym.sym == SDLK_UP) {
          vel.y = 0;
        }
        if (event.key.keysym.sym == SDLK_DOWN) {
          vel.y = 0;
        }
      }
    }
  }
};

<<<<<<< HEAD
class WallHitSystem : public UpdateSystem {
  void run(float dT) {
    auto view = scene->r.view<PositionComponent, VelocityComponent, SpriteComponent>();

    for (auto e : view) {
      auto pos = view.get<PositionComponent>(e);
      auto spr = view.get<SpriteComponent>(e);
      auto& vel = view.get<VelocityComponent>(e);

      int newPosX = pos.x + vel.x * dT;
      int newPosY = pos.y + vel.y * dT;

      if (newPosX < 0 || newPosX + spr.width > 1024) {
        vel.x *= -1.1;
      }

      if (newPosY < 0 || newPosY + spr.height > 768) {
        vel.y *= -1.1;
      }
    }
  }
};

class SpriteAnimationSystem : public UpdateSystem {
  void run(float dT) override {
    auto view = scene -> r.view<SpriteComponent>();
    Uint32 now = SDL_GetTicks();
    
    for(auto e: view){
      auto& spr = view.get<SpriteComponent>(e);

      if(spr.animationFrames > 0){
        if(spr.lastUpdate == 0){
          spr.lastUpdate = now;
          continue;
        }

        float timeSinceLastUpdate = now - spr.lastUpdate;

        int lastFrame = spr.animationFrames - 1;

        int framesUpdate = timeSinceLastUpdate / spr.animationDuration * spr.animationFrames;

        if(framesUpdate > 0){
          spr.xIndex += framesUpdate;
          spr.xIndex %= spr.animationFrames;          
          spr.lastUpdate = now;
        }
      }
    }
  }
};

class SpriteRenderSystem : public RenderSystem {
  void run(SDL_Renderer* renderer) {
    auto view = scene->r.view<PositionComponent, SpriteComponent>();
    for (auto e : view) {
      auto pos = view.get<PositionComponent>(e);
      auto spr = view.get<SpriteComponent>(e);

      Texture* texture = TextureManager::GetTexture(spr.filename);
      SDL_Rect clip = { spr.xIndex * spr.width, 0, spr.width, spr.height};
      texture -> render(scene->renderer, pos.x, pos.y, spr.width * spr.scale, spr.height * spr.scale, &clip);
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

class TextureSetupSystem : public SetupSystem {
  void run() {
    auto view = scene->r.view<TextureComponent>();
    for (auto e : view) {
      auto tex = view.get<TextureComponent>(e);
      TextureManager::LoadTexture(tex.filename, scene->renderer);
    }
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

=======
>>>>>>> origin/backgrounds

class DemoGame : public Game {
public:
  Scene* sampleScene;
  entt::registry r;

public:
  DemoGame()
  : Game("SAMPLE", 1024, 768)
  { }

  void setup() {
    printf("HELLO WORLD\n");  
    sampleScene = new Scene("SAMPLE SCENE", r, renderer);
    addSetupSystem<SquareSpawnSetupSystem>(sampleScene);
    addSetupSystem<BackgroundSetupSystem>(sampleScene);
    addSetupSystem<TilemapSetupSystem>(sampleScene);
    /* addSetupSystem<AutoTilingSetupSystem>(sampleScene); */
    addSetupSystem<AdvancedAutoTilingSetupSystem>(sampleScene);
    addSetupSystem<TextureSetupSystem>(sampleScene);
    addEventSystem<MovementInputSystem>(sampleScene);
    addUpdateSystem<SpriteMovementSystem>(sampleScene);
    addUpdateSystem<MovementSystem>(sampleScene);
    addUpdateSystem<SpriteAnimationSystem>(sampleScene);
    addRenderSystem<SpriteRenderSystem>(sampleScene);
    addRenderSystem<TilemapRenderSystem>(sampleScene);

    setScene(sampleScene);
  }
}; 