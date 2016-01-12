
#include "predict.h"

predict_cascade_t* load_adaboost(const char* file) {
  predict_cascade_t* cascade = alloc_predict_cascade();
  printf("CHEMIN AVANT REALPATH : %s\n", file );

  FILE *fp = fopen(file, "r");

  char line[255];
  while (fgets(line, 255, fp) != NULL) {
    printf("Ligne : %s\n", line);

    int t;
    int feature;
    double alpha;
    Int64 threshold;
    int parity;
    int i;
    int j;
    int w;
    int h;
    int charac;
    int n = sscanf(line, "%d feature=%d alpha=%lf threshold=%lli parity=%d j=%d i=%d w=%d h=%d charac=%d", &t, &feature, &alpha, &threshold, &parity, &j, &i, &w, &h, &charac);
    printf("Premier n : %d\n", n);
    if (n != 10) {
      double strong_threshold;
      n = sscanf(line, "layer=%d strong_threshold=%lf nb_features=%d", &(cascade->nb_layers), &strong_threshold, &t);
      printf("n : %d\n", n);

      assert(n == 3);
      cascade = add_layer_in_cascade(t, cascade);
      cascade->layers[cascade->nb_layers - 1]->strong_threshold = strong_threshold;
      continue;
    }
    cascade->layers[cascade->nb_layers - 1]->j[t] = j;
    cascade->layers[cascade->nb_layers - 1]->i[t] = i;
    cascade->layers[cascade->nb_layers - 1]->w[t] = w;
    cascade->layers[cascade->nb_layers - 1]->h[t] = h;
    cascade->layers[cascade->nb_layers - 1]->charac[t] = charac;
    cascade->layers[cascade->nb_layers - 1]->features[t] = feature;
    cascade->layers[cascade->nb_layers - 1]->alphas[t] = alpha;
    cascade->layers[cascade->nb_layers - 1]->thresholds[t] = threshold;
    cascade->layers[cascade->nb_layers - 1]->parities[t] = parity;
  }
  return cascade;
}

void test_features(int feature) {
  SDL_Surface* image =
    load_image("dataset/newface24/face24_001000.bmp");
  features_array_t* features = alloc_features_array();
  integral_image_t* integral_image = alloc_integral_image(image->w, image->h);
  convert_to_integral_image(image, integral_image);
  compute_haar_features_vector(features, integral_image);
  Int64 sum = 0;
  for (int i = 5; i < 12; ++i) {
    for (int j = 2; j < 5; ++j) {
      sum += getpixel(image, i, j);
    }
  }
  printf("%lli == %lli\n", sum, calculate_area(5, 2, 12, 5, integral_image));
  printf("size=%d features[%d]=%lli\n", features->size, feature, features->features[feature].value);
  free_features_array(features);
  free_integral_image(integral_image);
  SDL_FreeSurface(image);
}

void print_features() {
  predict_cascade_t* cascade = load_adaboost("adaboost_test2.txt");
  for (int k = 0; k < cascade->nb_layers; ++k) {
    printf("layer %d\n", k);
    for (int t = 0; t < cascade->layers[k]->T; ++t) {
      SDL_Surface* image =
        //load_image("dataset/nonface24/nonface24_002000.bmp");
        load_image("dataset/newface24/face24_002000.bmp");
      features_array_t* features = alloc_features_array();
      integral_image_t* integral_image = alloc_integral_image(image->w, image->h);
      convert_to_integral_image(image, integral_image);
      compute_haar_features_vector(features, integral_image);
      cascade_t* layer = alloc_cascade();
      layer->nb_features = cascade->layers[k]->T;
      layer->alphas = cascade->layers[k]->alphas;
      layer->thresholds = cascade->layers[k]->thresholds;
      layer->parities = cascade->layers[k]->parities;
      layer->features = cascade->layers[k]->features;
      strong_classify(features, layer);
      int i = features->features[cascade->layers[k]->features[t]].i;
      int h = features->features[cascade->layers[k]->features[t]].h;
      int j = features->features[cascade->layers[k]->features[t]].j;
      int w = features->features[cascade->layers[k]->features[t]].w;
      int charac = features->features[cascade->layers[k]->features[t]].charac;
      printf("  t=%d feature=%d value=%lli threshold=%lli\n  i=%d j=%d w=%d h=%d nb=%d\n", t,
             cascade->layers[k]->features[t],
             features->features[cascade->layers[k]->features[t]].value,
             cascade->layers[k]->thresholds[t],
             i, j, w, h, charac);
      Uint32 wpixel = SDL_MapRGB(image->format, 255, 255, 255);
      Uint32 bpixel = SDL_MapRGB(image->format, 0, 0, 0);
      switch (charac) {
      case 1:
        for (int height = i; height < i + h; ++height) {
          for (int width = j; width < j + w; ++width) {
            putpixel(image, width, height, wpixel);
          }
        }
        for (int height = i; height < i + h; ++height) {
          for (int width = j + w; width < j + 2 * w; ++width) {
            putpixel(image, width, height, bpixel);
          }
        }
        break;
      case 2:
        for (int height = i; height < i + h; ++height) {
          for (int width = j; width < j + w; ++width) {
            putpixel(image, width, height, wpixel);
          }
        }
        for (int height = i; height < i + h; ++height) {
          for (int width = j + w; width < j + 2 * w; ++width) {
            putpixel(image, width, height, bpixel);
          }
        }
        for (int height = i; height < i + h; ++height) {
          for (int width = j + 2 * w; width < j + 3 * w; ++width) {
            putpixel(image, width, height, wpixel);
          }
        }
        break;
      case 3:
        for (int height = i; height < i + h; ++height) {
          for (int width = j; width < j + w; ++width) {
            putpixel(image, width, height, wpixel);
          }
        }
        for (int height = i + h; height < i + 2 * h; ++height) {
          for (int width = j; width < j + w; ++width) {
            putpixel(image, width, height, bpixel);
          }
        }
        break;
      case 4:
        for (int height = i; height < i + h; ++height) {
          for (int width = j; width < j + w; ++width) {
            putpixel(image, width, height, wpixel);
          }
        }
        for (int height = i + h; height < i + 2 * h; ++height) {
          for (int width = j; width < j + w; ++width) {
            putpixel(image, width, height, bpixel);
          }
        }
        for (int height = i + 2 * h; height < i + 3 * h; ++height) {
          for (int width = j; width < j + w; ++width) {
            putpixel(image, width, height, wpixel);
          }
        }
        break;
      case 5:
        for (int height = i; height < i + h; ++height) {
          for (int width = j; width < j + w; ++width) {
            putpixel(image, width, height, wpixel);
          }
        }
        for (int height = i + h; height < i + 2 * h; ++height) {
          for (int width = j; width < j + w; ++width) {
            putpixel(image, width, height, bpixel);
          }
        }
        for (int height = i; height < i + h; ++height) {
          for (int width = j + w; width < j + 2 * w; ++width) {
            putpixel(image, width, height, bpixel);
          }
        }
        for (int height = i + h; height < i + 2 * h; ++height) {
          for (int width = j + w; width < j + 2 * w; ++width) {
            putpixel(image, width, height, wpixel);
          }
        }
        break;
      }
      display_image(image);
      free_features_array(features);
      free_integral_image(integral_image);
      SDL_FreeSurface(image);
    }
  }
}

static SDL_Surface* extract_image(SDL_Surface *image, int positionX, int positionY,
                                  int largeur, int hauteur) {
  SDL_Rect positionBlit;
  positionBlit.x = 0;
  positionBlit.y = 0;

  SDL_Rect positionExtraction;
  positionExtraction.x = positionX;
  positionExtraction.y = positionY;
  positionExtraction.w = largeur;
  positionExtraction.h = hauteur;

  SDL_Surface *nouvelleImageTemp = SDL_CreateRGBSurface(SDL_HWSURFACE, largeur, hauteur, 32, 0, 0, 0, 0);
  SDL_Surface *nouvelleImage = NULL;

  SDL_BlitSurface(image, &positionExtraction, nouvelleImageTemp, &positionBlit);

  nouvelleImage = nouvelleImageTemp;

  if (nouvelleImage == NULL) {
    fprintf(stderr, "Impossible d'extraire l'image");
    exit(EXIT_FAILURE);
  }

  return nouvelleImage;
}

static SDL_Surface* scale_surface(SDL_Surface *Surface, Uint16 Width, Uint16 Height) {
  if (!Surface || !Width || !Height)
    return 0;

  SDL_Surface *_ret = SDL_CreateRGBSurface(Surface->flags, Width, Height,
                      Surface->format->BitsPerPixel, Surface->format->Rmask,
                      Surface->format->Gmask, Surface->format->Bmask,
                      Surface->format->Amask);

  double _stretch_factor_x = (double) Width / (double) Surface->w;
  double _stretch_factor_y = (double) Height / (double) Surface->h;

  for (Sint32 y = 0; y < Surface->h; y++)
    for (Sint32 x = 0; x < Surface->w; x++)
      for (Sint32 o_y = 0; o_y < _stretch_factor_y; ++o_y)
        for (Sint32 o_x = 0; o_x < _stretch_factor_x; ++o_x)
          putpixel(_ret, (Sint32) (_stretch_factor_x * x) + o_x,
                   (Sint32) (_stretch_factor_y * y) + o_y, getpixel(Surface, x, y));
  return _ret;
}

void scan_image(SDL_Surface* image) {
  predict_cascade_t* cascade = load_adaboost("../SDL/adaboost_test2.txt");
  features_array_t* features = alloc_features_array();
  Uint32 red_pixel = SDL_MapRGB(image->format, 0, 255, 0);
  for (int i = 0; i < 162336; ++i) {
    feature_t f;
    f.i = f.j = f.w = f.h = f.charac = f.value = -1;
    add_feature(f, features);
  }

  const int padding = 2;
  for (int scale = 24; scale < 100; scale *= 1.25) {
    for (int i = 0; i < image->w - scale; i += padding) {
      for (int j = 0; j < image->h - scale; j += padding) {
        SDL_Surface* sub_window = extract_image(image, i, j, scale, scale);
        convert_to_greyscale(sub_window);
        //if (scale != 24) {
        sub_window = scale_surface(sub_window, 24, 24);
        //}
        integral_image_t* integral_image = alloc_integral_image(sub_window->w, sub_window->h);
        convert_to_integral_image(sub_window, integral_image);
        SDL_FreeSurface(sub_window);

        int k = 0;
        cascade_t layer;
        printf("\n=============================\n\n");
        printf("x=%d y=%d\n", i, j);
        while (k < cascade->nb_layers) {
          compute_cascade_features(features, cascade->layers[k], integral_image);
          layer.nb_features = cascade->layers[k]->T;
          layer.alphas = cascade->layers[k]->alphas;
          layer.thresholds = cascade->layers[k]->thresholds;
          layer.parities = cascade->layers[k]->parities;
          layer.features = cascade->layers[k]->features;
          layer.strong_threshold = cascade->layers[k]->strong_threshold;
          if (!strong_classify(features, &layer)) {
            printf("non face layer=%d\n", k);
            break;
          }
          ++k;
        }

        free_integral_image(integral_image);

        if (k == cascade->nb_layers) {
          printf("---------------------------------------------------------detected\n");
          for (int x = i; x < i + scale; ++x) {
            putpixel(image, x, j, red_pixel);
          }
          for (int y = j; y < j + scale; ++y) {
            putpixel(image, i, y, red_pixel);
          }
          for (int x = i; x < i + scale; ++x) {
            putpixel(image, x, j + scale, red_pixel);
          }
          for (int y = j; y < j + scale; ++y) {
            putpixel(image, i + scale, y, red_pixel);
          }
        }
      }
    }
  }
  SDL_SaveBMP(image, "../SDL/detection.bmp");
  free_features_array(features);
  free_predict_cascade(cascade);
}

/* int main() { */
/*   init_sdl(); */

/*   SDL_Surface* image = load_image("dataset/couple.jpg"); */
/*   scan_image(image); */
/*   display_image(image); */

/*   //print_features(); */

/*   //test_features(81554); */

/*   //test_strong_classifier(); */
/*   return 0; */
/* } */
