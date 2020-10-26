#include <err.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "pixel_operations.h"
#include "columns.h"

void init_sdl()
{
    // Init only the video part.
    // If it fails, die with an error message.
    if(SDL_Init(SDL_INIT_VIDEO) == -1)
        errx(1,"Could not initialize SDL: %s.\n", SDL_GetError());
}

SDL_Surface* load_image(char *path)
{
    SDL_Surface *img;

    // Load an image using SDL_image with format detection.
    // If it fails, die with an error message.
    img = IMG_Load(path);
    if (!img)
        errx(3, "can't load %s: %s", path, IMG_GetError());

    return img;
}

SDL_Surface* display_image(SDL_Surface *img)
{
    SDL_Surface *screen;

    // Set the window to the same size as the image
    screen = SDL_SetVideoMode(img->w, img->h, 0, SDL_SWSURFACE|SDL_ANYFORMAT);
    if (screen == NULL)
    {
        // error management
        errx(1, "Couldn't set %dx%d video mode: %s\n",
                img->w, img->h, SDL_GetError());
    }

    // Blit onto the screen surface
    if(SDL_BlitSurface(img, NULL, screen, NULL) < 0)
        warnx("BlitSurface error: %s\n", SDL_GetError());

    // Update the screen
    SDL_UpdateRect(screen, 0, 0, img->w, img->h);

    // return the screen for further uses
    return screen;
}

void wait_for_keypressed()
{
    SDL_Event event;

    // Wait for a key to be down.
    do
    {
        SDL_PollEvent(&event);
    } while(event.type != SDL_KEYDOWN);

    // Wait for a key to be up.
    do
    {
        SDL_PollEvent(&event);
    } while(event.type != SDL_KEYUP);
}

void SDL_FreeSurface(SDL_Surface *surface);

void binarize(SDL_Surface* screen_surface, SDL_Surface* image_surface)
{
    int w = image_surface -> w;// the width of img
    int h = image_surface -> h;// the height of img

    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < h; y++)
        {
            // Rappel : Uint8 = nombre signe allant de 0 a 255 inclus
            Uint32 pixel = get_pixel(image_surface, x, y);
            Uint8 r , g , b;
            // get colors of pixel
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
            // get the average of colors
            Uint32 average = (r + g + b) / 3; 
            if (average > 127) // because 255/2 = 127,5
            {
                // change pixel's color to white
		// if the average was greater than 127
                pixel = SDL_MapRGB(image_surface->format, 255, 255, 255); 
                put_pixel(image_surface, x, y, pixel);
            }
            else
            {
                // same reason as before, but here the color will be black
                pixel = SDL_MapRGB(image_surface->format, 0, 0, 0);
                put_pixel(image_surface, x, y, pixel);
            }
        }
    }

    update_surface(screen_surface, image_surface);
}

void grayscale(SDL_Surface *screen_surface, SDL_Surface *image_surface)
{
  int width = image_surface->w;
  int height = image_surface->h;
  
  for (int x = 0; x < width; x++) // For each pixel of the image
  {
    for (int y = 0; y < height; y++)
    {
      Uint32 pixel = get_pixel(image_surface, x, y); // Get the pixel value.
      Uint8 r, g, b;
      SDL_GetRGB(pixel, image_surface->format, &r, &g, &b); // Get the RGB values of the pixel.
      Uint8 average = 0.3*r + 0.59*g + 0.11*b; // Compute the weighted average of the RGB values
      Uint32 gray_pixel = SDL_MapRGB(image_surface->format, average, average, average); // Create gray pixel
      put_pixel(image_surface, x, y, gray_pixel); // Put the gray pixel on the image.
    }
  }

  update_surface(screen_surface, image_surface); // Redraw surfaces.
}

void lines(SDL_Surface *screen_surface, SDL_Surface *image_surface)
{ 
  int width = image_surface->w;
  int height = image_surface->h;

  int line_spacing_start = 0;
  int line_spacing_end = 0;
  int average_line_spacing = 0;

  int space = 1; // equal to 0 if on the text and to 1 if on the space

  Uint32 pixel;
  Uint8 r, g, b;

  Uint32 red_pixel = SDL_MapRGB(image_surface->format, 255, 0, 0);

  int array_with_lines[height/2];
  int array_index = 0;

  for (int y = 0; y < height; y++) // parcourir toutes les lignes
    {
      for (int x = 0; x < width; x++) // parcourir les colonnes
	{
	  pixel = get_pixel(image_surface, x, y); // recuperer le pixel
	  SDL_GetRGB(pixel, image_surface->format, &r, &g, &b); // recuperer ses donnees RGB
	  

	  if (!space) // si sur le texte :
	    {
	      if (!r) // - on rencontre un pixel noir
		{
		  break;
		}
	      else if (x == width - 1) // - on rencontre une ligne blanche
		{
		  line_spacing_start = y;
		  space = 1;
		}
	    }
	  else // si sur l'espace :
	    if (!r) // si on rencontre un pixel noir sur un espace
	    {
	      line_spacing_end = y;
	      space = 0;
	      average_line_spacing = (line_spacing_start + line_spacing_end) / 2;
	      array_with_lines[array_index] = average_line_spacing;
	      array_index++;
	      for (int each = 0; each < width; each++)
		{
		  put_pixel(image_surface, each, average_line_spacing, red_pixel);
		}
	    }
	}
    }

  array_with_lines[array_index] = -1;
  array_with_lines[array_index + 1] = -1; // sert a rien mais au cas ou

  array_index = 0;
  while (array_with_lines[array_index+1] != -1)
    {
      columns(array_with_lines[array_index], array_with_lines[array_index+1], image_surface);
      array_index++;
    }

  update_surface(screen_surface, image_surface); // Redraw surfaces.
}

int main()
{
    SDL_Surface* image_surface;
    SDL_Surface* screen_surface;

    image_surface = load_image("my_image.jpg"); // Initialize the SDL.
    screen_surface = display_image(image_surface); // Display the image.

    wait_for_keypressed(); // Wait for a key to be pressed.
    grayscale(screen_surface, image_surface); // Convert the image to gray
 
    wait_for_keypressed(); // Wait for a key to be pressed.
    binarize(screen_surface, image_surface);

    wait_for_keypressed();
    lines(screen_surface, image_surface);
    
    wait_for_keypressed();
    SDL_FreeSurface(image_surface); // Free the image surface.
    SDL_FreeSurface(screen_surface); // Free the screen surface.

    return 0;
}
