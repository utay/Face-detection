#ifndef _TYPES_H
# define _TYPES_H

# include <SDL/SDL.h>
# include "sdl.h"

typedef long long int Int64;

//integral image
typedef struct {
  Int64 **integral_image;
  int w;
  int h;
} integral_image_t;

integral_image_t* alloc_integral_image(int w, int h);
void free_integral_image(integral_image_t *image);

//features
typedef struct {
  char charac;
  char i;
  char j;
  char w;
  char h;
  Int64 value;
} feature_t;

typedef struct {
  feature_t *features;
  int size;
} features_array_t;

features_array_t* alloc_features_array();
void free_features_array(features_array_t* array);
void add_feature(feature_t f, features_array_t* array);

//image data set
typedef struct {
  features_array_t* features;
  char is_face;
  double weight;
  char consider;
} training_image_t;

training_image_t* alloc_training_image(char is_face);
void free_training_image(training_image_t* training_image);

typedef struct {
  training_image_t** images;
  int size_faces;
  int size_non_faces;
  int size;
  int capacity;
} image_dataset_t;

image_dataset_t* alloc_image_dataset();
void free_image_dataset(image_dataset_t* dataset);
void add_training_image(SDL_Surface* image, int is_face, image_dataset_t* dataset);
image_dataset_t* fill_dataset(image_dataset_t* dataset);

//adaboost
typedef struct {
  int size;
  double* alphas;
  double* weights;
  double* errors;
  int* parities;
  Int64* thresholds;
  training_image_t** images;
} adaboost_t;

adaboost_t* alloc_adaboost(int features);
void free_adaboost(adaboost_t* adaboost);

typedef struct {
  int nb_features;
  double* alphas;
  Int64* thresholds;
  int* features;
  int* parities;
  double false_positive_rate;
  double detection_rate;
  double strong_threshold;
  int* i;
  int* j;
  int* w;
  int* h;
  int* charac;
} cascade_t;

cascade_t* alloc_cascade();
void reinit_cascade(cascade_t* cascade, int nb_features);
void free_cascade(cascade_t* cascade);

typedef struct {
  int T;
  double strong_threshold;
  Int64* thresholds;
  double* alphas;
  int* features;
  int* parities;
  int* i;
  int* j;
  int* w;
  int* h;
  int* charac;
} predict_t;

predict_t* alloc_predict(int T);
void free_predict(predict_t* predict);

typedef struct {
  int nb_layers;
  predict_t** layers;
} predict_cascade_t;

predict_cascade_t* alloc_predict_cascade();
predict_cascade_t* add_layer_in_cascade(int t, predict_cascade_t* cascade);
void free_predict_cascade(predict_cascade_t* predict_cascade);

#endif
