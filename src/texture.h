#pragma once
#include <SDL3_image/SDL_image.h>

#include "base.h"
#include "renderer.h"

SDL_Surface *InitTexture(String path);
void DestroyTexture(SDL_Surface *texture);
Color GetPixelTexture(SDL_Surface *surface, i32 x, i32 y);
