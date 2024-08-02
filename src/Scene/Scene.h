#pragma once

#include <string>
//#include <entt/entt.hpp>

class Scene {
	public:
		Scene(const std::string& name);
		Scene() = delete; // Hace que esta funcion no permita que lo llamen
		~Scene(); // El destructor 
		// Delete es para liberar memoria. Delete llama al destructor
	
};