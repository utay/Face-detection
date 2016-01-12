#include "pixel_operations.h"
#include <assert.h>

static inline
Uint8* pixelref(SDL_Surface *surf, unsigned x, unsigned y) {
  int bpp = surf->format->BytesPerPixel;
  return (Uint8*)surf->pixels + y * surf->pitch + x * bpp;
}

Uint32 getpixel(SDL_Surface *surface, unsigned x, unsigned y) {
  Uint8 *p = pixelref(surface, x, y);
  switch(surface->format->BytesPerPixel) {
  case 1:
    return *p;
  case 2:
    return *(Uint16 *)p;
  case 3:
    if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
      return p[0] << 16 | p[1] << 8 | p[2];
    else
      return p[0] | p[1] << 8 | p[2] << 16;
  case 4:
    return *(Uint32 *)p;
  }
  return 0;
}

void putpixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel) {
  Uint8 *p = pixelref(surface, x, y);
  switch(surface->format->BytesPerPixel) {
  case 1:
    *p = pixel;
    break;
  case 2:
    *(Uint16 *)p = pixel;
    break;
  case 3:
    if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
      p[0] = (pixel >> 16) & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = pixel & 0xff;
    } else {
      p[0] = pixel & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = (pixel >> 16) & 0xff;
    }
    break;
  case 4:
    *(Uint32 *)p = pixel;
    break;
  }
}

void add_training_image(SDL_Surface* image, int is_face, image_dataset_t* dataset) {
  if (dataset->size >= dataset->capacity) {
    dataset->capacity *= 2;
    dataset->images = (training_image_t**) realloc(dataset->images, sizeof(training_image_t*) * dataset->capacity);
  }
  training_image_t* training_img = alloc_training_image(is_face);
  integral_image_t* integral_image = alloc_integral_image(image->w, image->h);
  convert_to_integral_image(image, integral_image);
  compute_haar_features_vector(training_img->features, integral_image);
  free_integral_image(integral_image);
  dataset->images[dataset->size] = training_img;
  if (is_face) {
    (dataset->size_faces)++;
  } else {
    (dataset->size_non_faces)++;
  }
  (dataset->size)++;
  SDL_FreeSurface(image);
}

void convert_to_greyscale(SDL_Surface *surface) {
  Uint8 r, g, b;
  Uint16 grey_scale;
  Uint32 grey_pixel;
  Uint32 pixel;
  for (int x = 0; x < surface->w; ++x) {
    for (int y = 0; y < surface->h; ++y) {
      pixel = getpixel(surface, x, y);
      SDL_GetRGB(pixel, surface->format, &r, &g, &b);
      grey_scale = 0.3 * r + 0.59 * g + 0.11 * b;
      grey_pixel = SDL_MapRGB(surface->format, grey_scale, grey_scale, grey_scale);
      putpixel(surface, x, y, grey_pixel);
    }
  }
}

int** create_mat(int h, int w){
    int** matrix = malloc(sizeof(int*)*w);
    for (int i =0; i<h;i++){
        matrix[i]=malloc(sizeof(int)*h);
    }
    return matrix;
}

SDL_Surface* mean_of_many_image(SDL_Surface *surface[]){
  Uint8 r, g, b;
  Uint32 mean_pixel;
  Uint32 pixel;
 // Uint8 r_mean[surface[1]->w][surface[1]->h], g_mean[surface[1]->w][surface[1]->h], b_mean[surface[1]->w][surface[1]->h]=0;
    int **r_mean = create_mat(surface[1]->h,surface[1]->w);
    int **g_mean = create_mat(surface[1]->h,surface[1]->w);
    int **b_mean = create_mat(surface[1]->h,surface[1]->w);
  int nb_surface=0;

  //On stock dans une matrice de la taille de la première surface la somme des valeurs des pixels des différentes images donnés paramètre.
  int j=0;
  while(surface[j]!=NULL)
    {
      for (int x = 0; x < surface[j]->w; ++x) {
        for (int y = 0; y < surface[j]->h; ++y) {
           pixel = getpixel(surface[j], x, y);
          SDL_GetRGB(pixel, surface[j]->format, &r, &g, &b);
          r_mean[x][y]+=r;
          g_mean[x][y]+=g;
          b_mean[x][y]+=b;
        }
      }
      nb_surface++;
      j++;
    }

    //Ici on calcul la moy pour chaque pixels
    for (int x = 0; x < surface[1]->w; ++x) {
        for (int y = 0; y < surface[1]->h; ++y) {
          r_mean[x][y]=r_mean[x][y]/nb_surface;
          g_mean[x][y]=g_mean[x][y]/nb_surface;
          b_mean[x][y]=b_mean[x][y]/nb_surface;
        }
      }

  //On "colle" les pixels sur la première image
  for (int x = 0; x < surface[1]->w; ++x) {
    for (int y = 0; y < surface[1]->h; ++y) {
      mean_pixel = SDL_MapRGB(surface[1]->format, **r_mean, **g_mean, **b_mean);
      putpixel(surface[1], x, y, mean_pixel);
    }
  }

  return surface[1];
}

static Int64 sum_pixel(int x, int y, SDL_Surface *surface,  integral_image_t *image) {
  Int64 sum = 0;
  sum += getpixel(surface, x, y);
  if (y > 0) {
    sum += image->integral_image[x][y - 1];
  }
  if (x > 0) {
    sum += image->integral_image[x - 1][y];
  }
  if (x > 0 && y > 0) {
    sum -= image->integral_image[x - 1][y - 1];
  }
  return sum;
}

void convert_to_integral_image(SDL_Surface *surface, integral_image_t *image) {
  Int64 sum_pix = 0;
  for (int x = 0; x < surface->w; ++x) {
    for (int y = 0; y < surface->h; ++y) {
      sum_pix = sum_pixel(x, y, surface, image);
      image->integral_image[x][y] = sum_pix;
    }
  }
}

Int64 calculate_area(int x0, int y0, int x1, int y1, const integral_image_t *image) {
  Int64 D = image->integral_image[x1 - 1][y1 - 1];
  Int64 B = y0 == 0 ? 0 : image->integral_image[x1 - 1][y0 - 1];
  Int64 C = x0 == 0 ? 0 : image->integral_image[x0 - 1][y1 - 1];
  Int64 A = x0 == 0 || y0 == 0 ? 0 : image->integral_image[x0 - 1][y0 - 1];
  return D - B - C + A;
}

Int64 f1(int j, int i, int w, int h, const integral_image_t* image) {
  Int64 value = 0;
  Int64 s1 = calculate_area(j, i, j + w, i + h, image);
  Int64 s2 = calculate_area(j + w, i, j + 2*w, i + h, image);
  value = s1 - s2;
  return value;
}

Int64 f2(int j, int i, int w, int h, const integral_image_t* image) {
  Int64 value = 0;
  Int64 s1 = calculate_area(j, i, j + w, i + h, image);
  Int64 s2 = calculate_area(j + w, i, j + 2*w, i + h, image);
  Int64 s3 = calculate_area(j + 2*w, i, j + 3*w, i + h, image);
  value = s1 - s2 + s3;
  return value;
}

Int64 f3(int j, int i, int w, int h, const integral_image_t* image) {
  Int64 value = 0;
  Int64 s1 = calculate_area(j, i, j + w, i + h, image);
  Int64 s2 = calculate_area(j, i + h, j + w, i + 2*h, image);
  value = s1 - s2;
  return value;
}

Int64 f4(int j, int i, int w, int h, const integral_image_t* image) {
  Int64 value = 0;
  Int64 s1 = calculate_area(j, i, j + w, i + h, image);
  Int64 s2 = calculate_area(j, i + h, j + w, i + 2*h, image);
  Int64 s3 = calculate_area(j, i + 2*h, j + w, i + 3*h, image);
  value = s1 - s2 + s3;
  return value;
}

Int64 f5(int j, int i, int w, int h, const integral_image_t* image) {
  Int64 value = 0;
  Int64 s1 = calculate_area(j, i, j + w, i + h, image);
  Int64 s2 = calculate_area(j, i + h, j + w, i + 2*h, image);
  Int64 s3 = calculate_area(j + w, i, j + 2*w, i + h, image);
  Int64 s4 = calculate_area(j + w, i + h, j + 2*w, i + 2*h, image);
  value = s1 - s2 - s3 + s4;
  return value;
}

//24 x 24 surface
features_array_t* compute_haar_features_vector(features_array_t* array, integral_image_t* image) {
  //feature 1
  for (int i = 0; i < 24; ++i) {
    for (int j = 0; j < 24; ++j) {
      for (int h = 1; (i + h) <= 24; ++h) {
        for (int w = 1; (j + 2*w) <= 24; ++w) {
          feature_t f;
          f.charac = 1;
          f.i = i;
          f.j = j;
          f.h = h;
          f.w = w;
          f.value = f1(j, i, w, h, image);
          add_feature(f, array);
        }
      }
    }
  }

  //feature 2
  for (int i = 0; i < 24; ++i) {
    for (int j = 0; j < 24; ++j) {
      for (int h = 1; (i + h) <= 24; ++h) {
        for (int w = 1; (j + 3*w) <= 24; ++w) {
          feature_t f;
          f.charac = 2;
          f.i = i;
          f.j = j;
          f.h = h;
          f.w = w;
          f.value = f2(j, i, w, h, image);
          add_feature(f, array);
        }
      }
    }
  }

  //feature 3
  for (int i = 0; i < 24; ++i) {
    for (int j = 0; j < 24; ++j) {
      for (int h = 1; (i + 2*h) <= 24; ++h) {
        for (int w = 1; (j + w) <= 24; ++w) {
          feature_t f;
          f.charac = 3;
          f.i = i;
          f.j = j;
          f.h = h;
          f.w = w;
          f.value = f3(j, i, w, h, image);
          add_feature(f, array);
        }
      }
    }
  }

  //feature 4
  for (int i = 0; i < 24; ++i) {
    for (int j = 0; j < 24; ++j) {
      for (int h = 1; (i + 3*h) <= 24; ++h) {
        for (int w = 1; (j + w) <= 24; ++w) {
          feature_t f;
          f.charac = 4;
          f.i = i;
          f.j = j;
          f.h = h;
          f.w = w;
          f.value = f4(j, i, w, h, image);
          add_feature(f, array);
        }
      }
    }
  }

  //feature 5
  for (int i = 0; i < 24; ++i) {
    for (int j = 0; j < 24; ++j) {
      for (int h = 1; (i + 2*h) <= 24; ++h) {
        for (int w = 1; (j + 2*w) <= 24; ++w) {
          feature_t f;
          f.charac = 5;
          f.i = i;
          f.j = j;
          f.h = h;
          f.w = w;
          f.value = f5(j, i, w, h, image);
          add_feature(f, array);
        }
      }
    }
  }
  return array;
}

void compute_cascade_features(features_array_t* features, predict_t* layer, integral_image_t* image) {
  for (int i = 0; i < layer->T; ++i) {
    Int64 value;
    switch (layer->charac[i]) {
    case 1:
      value = f1(layer->j[i], layer->i[i], layer->w[i], layer->h[i], image);
      break;
    case 2:
      value = f2(layer->j[i], layer->i[i], layer->w[i], layer->h[i], image);
      break;
    case 3:
      value = f3(layer->j[i], layer->i[i], layer->w[i], layer->h[i], image);
      break;
    case 4:
      value = f4(layer->j[i], layer->i[i], layer->w[i], layer->h[i], image);
      break;
    case 5:
      value = f5(layer->j[i], layer->i[i], layer->w[i], layer->h[i], image);
      break;
    default:
      assert(false);
    }
    features->features[layer->features[i]].value = value;
    features->features[layer->features[i]].i = layer->i[i];
    features->features[layer->features[i]].j = layer->j[i];
    features->features[layer->features[i]].w = layer->w[i];
    features->features[layer->features[i]].h = layer->h[i];
  }
}

static
int compute_hw(int x, int y, int coef) {
  int hw = 0;
  for (int k = 0;; ++k) {
    if ((k <= x) && (coef * k <= y)) {
      hw = k;
    } else if (!(k <= x) && !(coef * k <= y)) {
      //we can stop, k will never validate the conditions again
      break;
    }
  }
  return hw;
}

//side (image->w == image->h) >= 24
feature_t scale_feature(feature_t f, int side) {
  int org_size;
  switch (f.charac) {
  case 1:
    org_size = 2 * f.w * f.h;
    f.i = f.i * side / 24;
    f.j = f.j * side / 24;
    f.h = f.h * side / 24;
    f.w = compute_hw((1 + 2 * f.w * side / 24) / 2, side - f.j + 1, 2);
    f.value = (f.value * org_size) / (2 * f.w * f.h);
    break;
  case 2:
    org_size = 3 * f.w * f.h;
    f.i = f.i * side / 24;
    f.j = f.j * side / 24;
    f.h = f.h * side / 24;
    f.w = compute_hw((1 + 3 * f.w * side / 24) / 3, side - f.j + 1, 3);
    f.value = (f.value * org_size) / (3 * f.w * f.h);
    break;
  case 3:
    org_size = 2 * f.w * f.h;
    f.i = f.i * side / 24;
    f.j = f.j * side / 24;
    f.w = f.w * side / 24;
    f.h = compute_hw((1 + 2 * f.h * side / 24) / 2, side - f.i + 1, 2);
    f.value = (f.value * org_size) / (2 * f.w * f.h);
    break;
  case 4:
    org_size = 3 * f.w * f.h;
    f.i = f.i * side / 24;
    f.j = f.j * side / 24;
    f.w = f.w * side / 24;
    f.h = compute_hw((1 + 3 * f.h * side / 24) / 3, side - f.i + 1, 3);
    f.value = (f.value * org_size) / (3 * f.w * f.h);
    break;
  case 5:
    org_size = 4 * f.w * f.h;
    f.i = f.i * side / 24;
    f.j = f.j * side / 24;
    f.w = compute_hw((1 + 2 * f.w * side / 24) / 2, side - f.j + 1, 2);
    f.h = compute_hw((1 + 2 * f.h * side / 24) / 2, side - f.i + 1, 2);
    f.value = (f.value * org_size) / (4 * f.w * f.h);
    break;
  default:
    break;
  }
  return f;
}

double gaussian(int x, int y, double sigma) {
  return 1.0 / (2 * M_PI * sigma * sigma) *
    exp(-(x * x + y * y) / (2 * sigma * sigma));
}

//e x e image
double** create_filter(int e) {
  double** filter = (double**) malloc(sizeof(double*) * e);
  for (int i = 0; i < e; ++i) {
    filter[i] = (double*) malloc(sizeof(double) * e);
  }
  double sigma = 0.6 * sqrt((e / 24) * (e / 24) - 1);

  printf("sigma=%lf", sigma);

  for (int y = -(e / 2); y <= (e / 2); ++y) {
    for (int x = -(e / 2); x <= (e / 2); ++x) {
      filter[x + (e / 2)][y + (e / 2)] = gaussian(x, y, sigma);
    }
  }

  for (int y = 0; y < e; ++y) {
    for(int x = 0; x < e; ++x) {
      printf("y=%d x=%d %lf\n", y, x, filter[x][y]);
    }
  }

  return filter;
}

#define MIN(A, B) (A) < (B) ? (A) : (B);
#define MAX(A, B) (A) > (B) ? (A) : (B);

//e x e image (e > 24)
SDL_Surface* downsampling_image(SDL_Surface* blur_image) {
  int e = blur_image->w;
  SDL_Surface* result = SDL_CreateRGBSurface(SDL_SWSURFACE, 24, 24, 32, 0, 0, 0, 0);
  for (int x = 0; x < 24; ++x) {
    for (int y = 0; y < 24; ++y) {
      double i = (e - 1) * (x + 1) / 25;
      double j = (e - 1) * (y + 1) / 25;
      int i_max = MIN(i + 1, e - 1);
      int i_min = MAX(0, i);
      int j_max = MIN(j + 1, e - 1);
      int j_min = MAX(0, j);
      Uint32 scale = 0.25 * (getpixel(blur_image, i_max, j_max) + getpixel(blur_image, i_min, j_max)
                             + getpixel(blur_image, i_min, j_min) + getpixel(blur_image, i_max, j_min));
      putpixel(result, x, y, scale);
    }
  }
  return result;
}
