#pragma once
#include <fmod/fmod.hpp>
#include <fmod/fmod_common.h>
#include <string>
#include <iostream>
#include "Engine/Systems.h"
#include "Engine/Components.h"
#include "Engine/Entity.h"
#include "Engine/Game.h"

enum class SoundType {
  BACKGROUND,
  EFFECT,
  DIALOGUE
};

struct SoundComponent {
  SoundType type;
  std::string filename;
  FMOD_MODE mode;
  FMOD::Sound* sound;
  FMOD::Channel* channel;
};

class SoundSetupSystem : public SetupSystem {
public:
  void run() override {
    auto view = scene->r.view<SoundComponent>();

    for (auto entity : view) {
      auto& soundComponent = view.get<SoundComponent>(entity);

      game->soundManager->createStream(
        soundComponent.filename.c_str(), 
        soundComponent.mode,
        nullptr,
        &soundComponent.sound
      );
    }
  }
};

class BackgroundMusicSetupSystem : public SetupSystem {
public:
  void run() override {
    Entity* bgMusic = scene->createEntity("BGMUSIC");
    bgMusic->addComponent<SoundComponent>(SoundType::BACKGROUND, "assets/Audio/Battlestations_ Pacific - Japanese Action Theme.mp3", FMOD_LOOP_NORMAL);
  }
};

class FxMusicSetupSystem : public SetupSystem {
public:
  void run() override {
    Entity* jumpMusic = scene->createEntity("JUMPMUSIC");
    jumpMusic->addComponent<SoundComponent>(SoundType::EFFECT, "assets/Audio/espada_sound.wav", FMOD_DEFAULT);
  }
};

class BackgroundMusicPlaySetupSystem : public SetupSystem {
public:
  void run() override {
    auto view = scene->r.view<SoundComponent>();

    for (auto entity : view) {
      auto& soundComponent = view.get<SoundComponent>(entity);
      if (soundComponent.type == SoundType::BACKGROUND) {
        if (soundComponent.channel) {
          soundComponent.channel->stop();
        }
        game->soundManager->playSound(soundComponent.sound, nullptr, false, &soundComponent.channel);
        soundComponent.channel->setVolume(1.0);
        soundComponent.channel->setMode(FMOD_LOOP_NORMAL);
      }
    }
  }
};
