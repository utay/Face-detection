#ifndef PIXEL_OPERATIONS_H_
# define PIXEL_OPERATIONS_H_

# include <stdlib.h>
# include <SDL.h>
# include "types.h"

Uint32 getpixel(SDL_Surface *surface, unsigned x, unsigned y);
void putpixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel);
void convert_to_greyscale(SDL_Surface *surface);
void convert_to_integral_image(SDL_Surface *surface, integral_image_t *image);
Int64 calculate_area(int x0, int y0, int x1, int y1, const integral_image_t *image);
features_array_t* compute_haar_features_vector(features_array_t* array, integral_image_t* image);
void compute_cascade_features(features_array_t* features, predict_t* layer, integral_image_t* image);
feature_t scale_feature(feature_t f, int side);
double** create_filter(int e);
SDL_Surface* downsampling_image(SDL_Surface* blur_image);

#endif
