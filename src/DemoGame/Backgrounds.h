#pragma once
#include <string>
#include "Engine/Systems.h"
#include "Engine/Components.h"
#include "Engine/Entity.h"
#include "Sprites.h"

struct BackgroundComponent {
  std::string filename;
};

class BackgroundSetupSystem : public SetupSystem {
public:
  void run() override {
    Entity* background = scene->createEntity("BACKGROUND");
    const std::string& bgfile = "assets/Backgrounds/Sky.png";
    background->addComponent<PositionComponent>(0, 0);
    background->addComponent<TextureComponent>(bgfile);
    background->addComponent<SpriteComponent>(bgfile, 1024, 768, 1, 3, 1500);
    background->addComponent<BackgroundComponent>(bgfile);
  }
};
