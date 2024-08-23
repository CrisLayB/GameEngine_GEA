#include "Engine/Game.h"
#include "Engine/Scene.h"
#include "Engine/Entity.h"
#include "Engine/Components.h"
#include "Engine/Systems.h"
#include "Engine/Graphics/TextureManager.h"
#include "Engine/Graphics/Texture.h"
#include <print>
#include <entt/entt.hpp>

struct TextureComponent {
  std::string filename;
};

struct BackgroundComponent {
  std::string filename;
};

struct PlayerComponent {
};

struct SpriteComponent {
  std::string filename;
  int width;
  int height;
  int scale = 1;
  int animationFrames = 0;
  int animationDuration = 0;
  Uint32 lastUpdate = 0;  
  int xIndex = 0;
  int yIndex = 0;
};

class SamuraiSpawnSetupSystem : public SetupSystem {
	void run(){
		Entity* samurai = scene->createEntity("SAMURAI", 10, 10);
		samurai->addComponent<PlayerComponent>();
		samurai->addComponent<VelocityComponent>(0);
		samurai->addComponent<TextureComponent>("assets/Sprites/cat.png");
    	samurai->addComponent<SpriteComponent>("assets/Sprites/cat.png", 8, 8, 10, 1, 1000);
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
      SDL_Rect clip = {
        spr.xIndex * spr.width,
        spr.yIndex * spr.height,
        spr.width,
        spr.height,
      };
      texture->render(scene->renderer, pos.x, pos.y, spr.width * spr.scale, spr.height * spr.scale, &clip);
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

		addSetupSystem<SamuraiSpawnSetupSystem>(sampleScene);
		//addSetupSystem<BackgroundSetupSystem>(sampleScene);
		addSetupSystem<TextureSetupSystem>(sampleScene);

		addUpdateSystem<SpriteAnimationSystem>(sampleScene);
		//addRenderSystem<BackgroundRenderSystem>(sampleScene);
		addRenderSystem<SpriteRenderSystem>(sampleScene);

		setScene(sampleScene);
	}
};