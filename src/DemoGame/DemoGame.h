#include "Engine/Game.h"
#include "Engine/Scene.h"
#include "Engine/Entity.h"
#include "Engine/Components.h"
#include "Engine/Systems.h"
#include "Engine/Graphics/TextureManager.h"
#include "Engine/Graphics/Texture.h"
#include <print>
#include <entt/entt.hpp>

struct SpriteComponent {
  int width;
  int height;
  SDL_Color color;
};

class SquareSpawnSetupSystem : public SetupSystem {
  void run() {
    Entity* square = scene->createEntity("SQUARE", 10, 10); 
    square->addComponent<VelocityComponent>(500, 500);
    square->addComponent<SpriteComponent>(100, 100, SDL_Color{255, 0, 0});

    Entity* square2 = scene->createEntity("SQUARE2", 924, 10); 
    square2->addComponent<VelocityComponent>(-400, -400);
    square2->addComponent<SpriteComponent>(100, 100, SDL_Color{0, 0, 255});
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


struct TextureComponent {
  std::string filename;
};

struct BackgroundComponent {
  std::string filename;
};

class BackgroundSetupSystem : public SetupSystem {
public:
  void run() override {
    Entity* background = scene->createEntity("BACKGROUND");
    const std::string& bgfile = "assets/Backgrounds/Sky.png";
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


class DemoGame : public Game {
public:
  Scene* sampleScene;
  entt::registry r;

public:
  DemoGame()
  : Game("SAMPLE", 1024, 768)
  { }

  void setup() {
    std::print("HELLO WORLD\n");  
    sampleScene = new Scene("SAMPLE SCENE", r, renderer);
    addSetupSystem<SquareSpawnSetupSystem>(sampleScene);
    addSetupSystem<BackgroundSetupSystem>(sampleScene);
    addSetupSystem<TextureSetupSystem>(sampleScene);

    addUpdateSystem<MovementSystem>(sampleScene);
    addUpdateSystem<WallHitSystem>(sampleScene);
    addRenderSystem<BackgroundRenderSystem>(sampleScene);
    addRenderSystem<SquareRenderSystem>(sampleScene);

    setScene(sampleScene);
  }
}; 