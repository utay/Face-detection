#include "types.h"

static const int INITIAL_CAPACITY = 1000;

//integral image
integral_image_t* alloc_integral_image(int w, int h) {
  integral_image_t *image = (integral_image_t*) malloc(sizeof(integral_image_t));
  image->w = w;
  image->h = h;
  image->integral_image = (Int64**) malloc(sizeof(Int64*) * w);
  for (int i = 0; i < w; ++i) {
    image->integral_image[i] = (Int64*) malloc(sizeof(Int64) * h);
  }
  return image;
}

void free_integral_image(integral_image_t *image) {
  for (int i = 0; i < image->w; ++i) {
    free(image->integral_image[i]);
  }
  free(image->integral_image);
  free(image);
}

//features
features_array_t* alloc_features_array() {
  features_array_t *array = (features_array_t*) malloc(sizeof(features_array_t));
  array->size = 0;
  array->features = NULL;
  return array;
}

void free_features_array(features_array_t* array) {
  free(array->features);
  free(array);
}

void add_feature(feature_t f, features_array_t* array) {
  array->features = (feature_t*) realloc(array->features, sizeof(feature_t) * (array->size + 1));
  array->features[array->size] = f;
  (array->size)++;
}

//training image
training_image_t* alloc_training_image(char is_face) {
  training_image_t* training_img = malloc(sizeof(training_image_t));
  training_img->features = alloc_features_array();
  training_img->is_face = is_face;
  training_img->consider = 1;
  return training_img;
}

void free_training_image(training_image_t* training_image) {
  free_features_array(training_image->features);
  free(training_image);
}

//image data set
image_dataset_t* alloc_image_dataset() {
  image_dataset_t* dataset = (image_dataset_t*) malloc(sizeof(image_dataset_t));
  dataset->capacity = INITIAL_CAPACITY;
  dataset->size_faces = 0;
  dataset->size_non_faces = 0;
  dataset->size = 0;
  dataset->images = (training_image_t**) malloc(sizeof(training_image_t*) * dataset->capacity);
  return dataset;
}

void free_image_dataset(image_dataset_t* dataset) {
  for (int i = 0; i < dataset->size; ++i) {
    free_training_image(dataset->images[i]);
  }
  free(dataset->images);
  free(dataset);
}

image_dataset_t* fill_dataset(image_dataset_t* dataset) {
  char path[255];

  int nb_images = 4000;
  for (int i = 3000; i < 3000 + nb_images; ++i) {
    sprintf(path,
            "/home/neo/Facificator/reconnaissance-faciale/SDL/dataset/newface24/face24_%06d.bmp", i);
    SDL_Surface* image = load_image(path);
    add_training_image(image, 1, dataset);
  }

  nb_images = 10000;
  for (int i = 3000; i < 3000 + nb_images; ++i) {
    sprintf(path,
            "/home/neo/Facificator/reconnaissance-faciale/SDL/dataset/nonface24/nonface24_%06d.bmp", i);
    SDL_Surface* image = load_image(path);
    add_training_image(image, 0, dataset);
  }

  return dataset;
}

//adaboost
adaboost_t* alloc_adaboost(int features) {
  adaboost_t* adaboost = (adaboost_t*) malloc(sizeof(adaboost_t));
  adaboost->size = features;
  adaboost->errors = (double*) malloc(sizeof(double) * features);
  adaboost->alphas = (double*) malloc(sizeof(double) * features);
  adaboost->thresholds = (Int64*) malloc(sizeof(Int64) * features);
  adaboost->parities = (int*) malloc(sizeof(int) * features);
  adaboost->images = malloc(sizeof(training_image_t*) * features);
  return adaboost;
}

void free_adaboost(adaboost_t* adaboost) {
  free(adaboost->alphas);
  free(adaboost->errors);
  free(adaboost->thresholds);
  free(adaboost->parities);
  free(adaboost->images);
  free(adaboost);
}


//cascade
cascade_t* alloc_cascade() {
  cascade_t* cascade = (cascade_t*) malloc(sizeof(cascade_t));
  cascade->nb_features = 0;
  cascade->alphas = NULL;
  cascade->thresholds = NULL;
  cascade->parities = NULL;
  cascade->features = NULL;
  cascade->strong_threshold = cascade->false_positive_rate = cascade->detection_rate = 0;
  return cascade;
}

void reinit_cascade(cascade_t* cascade, int nb_features) {
  cascade->nb_features = nb_features;
  cascade->alphas = (double*) realloc(cascade->alphas, sizeof(double) * nb_features);
  cascade->thresholds = (Int64*) realloc(cascade->thresholds, sizeof(Int64) * nb_features);
  cascade->parities = (int*) realloc(cascade->parities, sizeof(int) * nb_features);
  cascade->features = (int*) realloc(cascade->features, sizeof(int) * nb_features);
  cascade->strong_threshold = cascade->false_positive_rate = cascade->detection_rate = 0;
}

void free_cascade(cascade_t* cascade) {
  free(cascade->alphas);
  free(cascade->features);
  free(cascade->thresholds);
  free(cascade->parities);
  free(cascade);
}


//predict
predict_t* alloc_predict(int T) {
  predict_t* predict = (predict_t*) malloc(sizeof(predict_t));
  predict->T = T;
  predict->strong_threshold = 0;
  predict->alphas = (double*) malloc(sizeof(double) * T);
  predict->thresholds = (Int64*) malloc(sizeof(Int64) * T);
  predict->features = (int*) malloc(sizeof(int) * T);
  predict->parities = (int*) malloc(sizeof(int) * T);
  predict->i = malloc(sizeof(int) * T);
  predict->j = malloc(sizeof(int) * T);
  predict->w = malloc(sizeof(int) * T);
  predict->h = malloc(sizeof(int) * T);
  predict->charac = malloc(sizeof(int) * T);
  return predict;
}

void free_predict(predict_t* predict) {
  free(predict->alphas);
  free(predict->features);
  free(predict->thresholds);
  free(predict->parities);
  free(predict->i);
  free(predict->j);
  free(predict->w);
  free(predict->h);
  free(predict->charac);
  free(predict);
}

//predict cascade
predict_cascade_t* alloc_predict_cascade() {
  predict_cascade_t* cascade = malloc(sizeof(predict_cascade_t));
  cascade->nb_layers = 0;
  cascade->layers = NULL;
  printf("alloc_predict_cascade() %p\n", (void*) cascade);
  return cascade;
}

predict_cascade_t* add_layer_in_cascade(int t, predict_cascade_t* cascade) {
  predict_t* predict = alloc_predict(t);
  cascade->layers = realloc(cascade->layers, sizeof(predict_t*) * cascade->nb_layers);
  cascade->layers[cascade->nb_layers - 1] = predict;
  printf("add_layer() %p\n", (void*) cascade);
  return cascade;
}

void free_predict_cascade(predict_cascade_t* predict_cascade) {
  for (int i = 0; i < predict_cascade->nb_layers; ++i) {
    free_predict(predict_cascade->layers[i]);
  }
    printf("free_cascade() %p\n", (void*) predict_cascade);
  free(predict_cascade);
}
