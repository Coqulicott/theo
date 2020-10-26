#ifndef GRAYSCALE_H
#define GRAYSCALE_H

#include <stdlib.h>
#include <SDL.h>

void grayscale(SDL_surface *screen_surface, SDL_Surface *image_surface);
void binarize(SDL_Surface *screen_surface, SDL_surface *image_surface);
void lines(SDL_Surface *scree_surface, SDL_Surface *image_surface);

#endif
