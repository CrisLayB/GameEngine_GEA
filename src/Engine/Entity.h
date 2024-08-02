#pragma once
#include <entt/entt.hpp>
#include <print>
#include <utility>
#include "Scene.h"
#include "Components.h"

class Entity {
public:
  Entity(entt::entity h, Scene* s)
    : handle(h), scene(s)
  {    
  }

  ~Entity() {
    scene->r.destroy(handle);
    std::print("Enttiy Destroyed\n");
  }

  template<typename T>
  void addComponent(auto&&... args) {
    scene->r.emplace<T>(handle, std::forward<decltype(args)>(args)...);
  }

private:
  entt::entity handle;
  Scene* scene;
};
