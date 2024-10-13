#pragma once
#include <SDL2/SDL.h>
#include <iostream>
#include "DemoGame/Tilemap.h"
#include "Engine/Components.h"
#include "Engine/Systems.h"
/* #include "Tilemap.h" */
#include "Player.h"

enum class CollisionType {
  NONE,
  WALL,
  TRIGGER,
  ENEMY_DETECTION,
};

struct BoxColliderComponent {
  SDL_Rect rect;
  SDL_Color color;
  CollisionType collisionType = CollisionType::NONE;
  bool isTriggered = false;
};

class ColliderResetSystem : public UpdateSystem {
public:
  void run(float dT) override {
    auto view = scene->r.view<BoxColliderComponent>();

    for (auto entity : view) {
      auto &collider = view.get<BoxColliderComponent>(entity);

      collider.collisionType = CollisionType::NONE;
    }
  }
};

class ColliderRenderSystem : public RenderSystem {
public:
  void run(SDL_Renderer* renderer) override {
    auto view = scene->r.view<PositionComponent, BoxColliderComponent>();

    for (auto entity : view) {
      auto [position, collider] = view.get<PositionComponent, BoxColliderComponent>(entity);

      SDL_Rect renderRect = {
        position.x + collider.rect.x,
        position.y + collider.rect.y,
        collider.rect.w,
        collider.rect.h,
      };

      SDL_SetRenderDrawColor(renderer, collider.color.r, collider.color.g, collider.color.b, collider.color.a);
      SDL_RenderDrawRect(renderer, &renderRect);
    }
  }
};



class PlayerPowerUpCollisionDetectionSystem : public UpdateSystem {
public: 
  void run (float dT) override {
    auto playerView = scene->r.view<PlayerComponent, BoxColliderComponent, PositionComponent>();
    auto powerUpView = scene->r.view<PowerUpComponent, BoxColliderComponent, PositionComponent>();

    for (auto player : playerView) {
      auto position = playerView.get<PositionComponent>(player);
      auto& collider = playerView.get<BoxColliderComponent>(player);

      SDL_Rect playerRect = {
        position.x + collider.rect.x,
        position.y + collider.rect.y,
        collider.rect.w,
        collider.rect.h,
      };

      for (auto powerUp: powerUpView) {
        auto [pposition, pcollider] = powerUpView.get<PositionComponent, BoxColliderComponent>(powerUp);

        SDL_Rect powerUpRect = {
          pposition.x + pcollider.rect.x,
          pposition.y + pcollider.rect.y,
          pcollider.rect.w,
          pcollider.rect.h,
        };

        if (SDL_HasIntersection(&playerRect, &powerUpRect)) {
          collider.collisionType = CollisionType::TRIGGER; 
        }
      }
    }
  }
};

class PlayerIsInsideEnemyDetectionSystem : public UpdateSystem {
public: 
  void run(float dT) override {
    auto playerView = scene->r.view<PlayerComponent, BoxColliderComponent, PositionComponent>();
    auto enemyView = scene->r.view<EnemyComponent, BoxColliderComponent, PositionComponent>();

    for (auto player : playerView) {
      auto position = playerView.get<PositionComponent>(player);
      auto& collider = playerView.get<BoxColliderComponent>(player);

      SDL_Rect playerRect = {
        position.x + collider.rect.x,
        position.y + collider.rect.y,
        collider.rect.w,
        collider.rect.h,
      };

      for (auto enemy : enemyView) {
        auto [eposition, ecollider] = enemyView.get<PositionComponent, BoxColliderComponent>(enemy);

        SDL_Rect enemyRect = {
          eposition.x + ecollider.rect.x,
          eposition.y + ecollider.rect.y,
          ecollider.rect.w,
          ecollider.rect.h,
        };

        if (SDL_HasIntersection(&playerRect, &enemyRect)) {
          collider.collisionType = CollisionType::ENEMY_DETECTION;
          //velocity.x = 0;
          //velocity.y = 0;
        }
      }
    }
  }
};

class EnemyWillAttackPlayerCollisionSystem : public UpdateSystem {
public:
  void run(float dT) override {
    auto playerView = scene->r.view<PlayerComponent, BoxColliderComponent, VelocityComponent>();
    auto enemyView = scene->r.view<EnemyComponent>();

    Uint32 currentTime = SDL_GetTicks();

    for (auto player : playerView) {
      auto& playerCollider = playerView.get<BoxColliderComponent>(player);
      auto& velocity = playerView.get<VelocityComponent>(player);
      auto& playerStruct = playerView.get<PlayerComponent>(player);

      if (playerCollider.collisionType == CollisionType::ENEMY_DETECTION) {
        for (auto enemy : enemyView) {
          auto& enemyStruct = enemyView.get<EnemyComponent>(enemy);
          if (currentTime - enemyStruct.lastAttackTime >= 1500) {
            playerStruct.life--;
            enemyStruct.lastAttackTime = currentTime;

            std::cout << "Enemy attacked! Player life: " << playerStruct.life << std::endl;
            if(playerStruct.life <= 0){
              std::cout << "Game Over" << std::endl;
              scene->onStop();
            }
          }
        }
      }
    }
  }
};

class PlayerPowerUpCollisionSystem : public UpdateSystem {
public: 
  void run (float dT) override {
    auto playerView = scene->r.view<PlayerComponent, BoxColliderComponent, PositionComponent>();

    for (auto player : playerView) {
      auto& collider = playerView.get<BoxColliderComponent>(player);

      if (collider.collisionType == CollisionType::TRIGGER && !collider.isTriggered) {
        std::cout << "player collider with power up" << std::endl;
        collider.isTriggered = true;
      }
    }
  }
};

class PlayerEnemyCollisionDetectionSystem : public UpdateSystem {
public: 
  void run(float dT) override {
    auto playerView = scene->r.view<PlayerComponent, BoxColliderComponent, PositionComponent>();
    auto enemyView = scene->r.view<EnemyComponent, BoxColliderComponent, PositionComponent>();

    for (auto player : playerView) {
      auto position = playerView.get<PositionComponent>(player);
      auto& collider = playerView.get<BoxColliderComponent>(player);

      SDL_Rect playerRect = {
        position.x + collider.rect.x,
        position.y + collider.rect.y,
        collider.rect.w,
        collider.rect.h,
      };

      for (auto enemy : enemyView) {
        auto [eposition, ecollider] = enemyView.get<PositionComponent, BoxColliderComponent>(enemy);

        SDL_Rect enemyRect = {
          eposition.x + ecollider.rect.x,
          eposition.y + ecollider.rect.y,
          ecollider.rect.w,
          ecollider.rect.h,
        };

        if (SDL_HasIntersection(&playerRect, &enemyRect)) {
          std::cout << "perdiste" << std::endl;
        }
      }
    }
  }
};


class ShurikenEnemyCollisionSystem : public UpdateSystem {
public: 
  void run(float dT) override {
    auto shurikenView = scene->r.view<ShurikenComponent, BoxColliderComponent, PositionComponent>();
    auto enemyView = scene->r.view<EnemyComponent, BoxColliderComponent, PositionComponent>();

    for (auto shuriken : shurikenView) {
      auto sposition = shurikenView.get<PositionComponent>(shuriken);
      auto& scollider = shurikenView.get<BoxColliderComponent>(shuriken);

      SDL_Rect shurikenRect = {
        sposition.x + scollider.rect.x,
        sposition.y + scollider.rect.y,
        scollider.rect.w,
        scollider.rect.h,
      };

      for (auto enemy : enemyView) {
        auto eposition = enemyView.get<PositionComponent>(enemy);
        auto& ecollider = enemyView.get<BoxColliderComponent>(enemy);

        SDL_Rect enemyRect = {
          eposition.x + ecollider.rect.x,
          eposition.y + ecollider.rect.y,
          ecollider.rect.w,
          ecollider.rect.h,
        };

        if (SDL_HasIntersection(&shurikenRect, &enemyRect)) {
          std::cout << "shuriken hit enemy" << std::endl;
        }
      }
    }
  }
};
