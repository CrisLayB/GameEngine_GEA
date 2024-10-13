#pragma once

#include <random>

struct PlayerComponent {
  int speed = 200;
  short life = 7;
  short score = 0;
  bool isAttack = false;
  float lastAttackTime = 0.0f;
};

int generateRandomXPosition(int min = -500, int max = -300) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(min, max);
  return dist(gen);
};

void setEnemyPosition(entt::registry& registry, entt::entity enemy, int newY) {
  auto& position = registry.get<PositionComponent>(enemy);
  position.x = generateRandomXPosition();
  position.y = newY;
};

struct EnemyComponent {  
  int speed = 100;  
  short originalLife = 3;
  short life = originalLife;
  Uint32 lastAttackTime = 0;

  void setPosition(PositionComponent& pos) {
    pos.x = generateRandomXPosition();
    pos.y = 0;
  }
};

struct PlayerAttackEvent {
  entt::entity player;
};

struct ShurikenComponent {

};

struct PowerUpComponent {

};

struct Enemy {

};