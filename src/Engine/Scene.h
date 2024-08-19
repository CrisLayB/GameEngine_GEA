#pragma once

#include <string>
#include <entt/entt.hpp>
#include <SDL2/SDL.h>

class Entity;
class SetupSystem;  // loading
class EventSystem;  // for events + keyboard
class RenderSystem;  // render
class UpdateSystem;  // updates variables

class Scene {
public:
  std::vector<SetupSystem*> setupSystems;
  std::vector<EventSystem*> eventSystems;
  std::vector<RenderSystem*> renderSystems;
  std::vector<UpdateSystem*> updateSystems;

  Scene(const std::string&, entt::registry&, SDL_Renderer*);
  ~Scene();

  Entity* createEntity(const std::string&);
  Entity* createEntity(const std::string&,int,int);

  void setup();
  void update(float dT);
  void render(SDL_Renderer* renderer);
  void processEvents(SDL_Event e);

  entt::registry& r;
  std::string name;
  std::function<void()> onStop;
  SDL_Renderer* renderer;
};
