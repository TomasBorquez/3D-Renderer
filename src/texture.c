#include "texture.h"

SDL_Surface *InitTexture(String path) {
  SDL_Surface *surface = IMG_Load(path.data);
  if (surface == NULL) {
    LogError("Could not load texture for %s", path.data);
    abort();
  }
  return surface;
}

Color GetPixelTexture(SDL_Surface *surface, i32 x, i32 y) {
  Color result = {0};
  SDL_ReadSurfacePixel(surface, x, y, &result.r, &result.g, &result.b, &result.a);
  return result;
}

void DestroyTexture(SDL_Surface *surface) {
  SDL_DestroySurface(surface);
}
