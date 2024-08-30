#include "Texture.h"
#include <SDL_render.h>
#include <SDL_surface.h>

Texture::Texture()
  : texture(nullptr), width(0), height(0) {
}

Texture::~Texture() {
  free();
}

void Texture::load(const std::string& path, SDL_Renderer* renderer) {
  free();
  SDL_Surface* loadedSurface = IMG_Load(path.c_str());

  texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
  if (!texture) {
    printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
  }

  width = loadedSurface->w;
  height = loadedSurface->h;

  /* SDL_FreeSurface(loadedSurface); */
}

void Texture::free() {
  if (texture != nullptr) {
    SDL_DestroyTexture(texture);
    width = 0;
    height = 0;
  }
}

void Texture::render(SDL_Renderer* renderer, int x, int y, int w, int h, SDL_Rect* clip) {
  int rWidth = width;
  int rHeight = height;

  if(w != 0){
    rWidth = w;
  }
  if(h != 0){
    rHeight = h;
  }

  SDL_Rect renderQuad = SDL_Rect{ x, y, rWidth, rHeight };

  //if (clip != nullptr) {
  //  srcrect.w = clip->w;
  //  srcrect.h = clip->h;
  //}

  SDL_RenderCopy(renderer, texture, clip, &renderQuad);
}