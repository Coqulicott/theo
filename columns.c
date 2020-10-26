#include "columns.h"
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "pixel_operations.h"

void columns(int line_start, int line_end, SDL_Surface *image_surface)
{
  int width = image_surface->w;

  Uint32 pixel;
  Uint8 r, g, b;

  Uint32 red_pixel = SDL_MapRGB(image_surface->format, 255, 0, 0);
  
  int array[width/2]; // array with the coordinates of vertical lines
  int array_index = 0;

  int space = 1; // equal to 1 if on the space and equal to 0 if on the letter

  for (int x = 0; x < width; x++) // browse horizontally
    {
      for (int y = line_start; y < line_end; y++) // browse vertically
	{
	  pixel = get_pixel(image_surface, x, y); // get the pixel
	  SDL_GetRGB(pixel, image_surface->format, &r, &g, &b); // get the RGB values of the pixel
	  
	  if (space)
	    {
	      if (!r) // if black pixel when we are on the space
		{
		  array[array_index] = x-1; // adding coord of the beginning of the letter to the array
		  array_index++;
		  space = 0;
		  break;
		}
	    }
	  else
	    {
	      if (!r) // if black pixel when we are not on the space
		{
		  break;
		}
	      if (y == line_end - 1)
		{
		  array[array_index] = x; // adding coord of the end of the letter to the array
		  array_index++;
		  space = 1;
		}
	    }
	}
    }

  array[array_index+1] = -1; // sert a rien mais au cas ou
  array[array_index] = -1; // value of the array showing the end

  int i = 0;
  int sum = 0;
  int number_of_letters = 0;
  while (array[i] != -1) // browse the array to find the average letter width
    {
      number_of_letters += 1;
      sum += (array[i+1] - array[i]);
      i += 2;
    }
  float average_letter_width = (float)sum / (float)number_of_letters;

  float space_estimation = average_letter_width / 2.5; // if lower than 40% of a letter then not a space

  i = 1;

  // tracer un trait au debut

  
  while (array[i] != -1)
    {
      if ((float)(array[i+1] - array[i]) < space_estimation) // check the spacing
	{
	  int final_line = (array[i+1] + array[i]) / 2;
	  for (int y = line_start; y < line_end; y++)
	    {
	      put_pixel(image_surface, final_line, y, red_pixel);
	    }
	}
      else
	{
	  for (int y = line_start; y < line_end; y++)
	    {
	      put_pixel(image_surface, array[i], y, red_pixel);
	      put_pixel(image_surface, array[i+1], y, red_pixel);
	    }
	}
      i += 2;
    }
}
