#pragma once
#include <fmod/fmod.hpp>
#include <string>
#include <iostream>
#include "Engine/Systems.h"
#include "Engine/Components.h"
#include "Engine/Entity.h"
#include "Engine/Game.h"

enum class SoundType {
  BACKGROUND,
  EFFECT
};

enum class NameSound {
  BACKGROUND,
  ATTACK,
  DAMAGE,
};

struct SoundComponent {
  SoundType type;
  NameSound identifier;
  FMOD::Sound* sound;
  FMOD::Channel* channel;
};

class SoundSetupSystem : public SetupSystem {
public:
  void run() override {
    // Load background music
    Entity* bgMusic = scene->createEntity("BACKGROUND_MUSIC");
    FMOD::Sound* music;
    game->soundManager->createStream("assets/Audio/Battlestations_ Pacific - Japanese Action Theme.mp3", FMOD_LOOP_NORMAL, nullptr, &music);
    bgMusic->addComponent<SoundComponent>(SoundType::BACKGROUND, NameSound::BACKGROUND, music, nullptr);

    // Load attack sound effect
    Entity* attackSound = scene->createEntity("ATTACK_SOUND");
    FMOD::Sound* attack;
    game->soundManager->createSound("assets/Audio/espada_sound.wav", FMOD_DEFAULT, nullptr, &attack);
    attackSound->addComponent<SoundComponent>(SoundType::EFFECT, NameSound::ATTACK, attack, nullptr);
  }
};

class BackgroundMusicSystem : public SetupSystem {
public:
  void run() override {
    FMOD::System* soundManager = game->soundManager;

    auto view = scene->r.view<SoundComponent>();
    for (auto entity : view) {
      auto& sound = view.get<SoundComponent>(entity);
      if (sound.type == SoundType::BACKGROUND) {
        if (sound.channel) {
          // If the channel exists, stop the current playback
          sound.channel->stop();
        }
        // Start playing the background music
        soundManager->playSound(sound.sound, nullptr, false, &sound.channel);
        // Set the channel mode to loop
        sound.channel->setMode(FMOD_LOOP_NORMAL);
        break;
      }
    }
  }
};
