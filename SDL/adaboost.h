#include <stdlib.h>
#include <SDL.h>
#include "pixel_operations.h"

void adaboost_train_feature(adaboost_t* adaboost, int feature, const image_dataset_t* dataset);
void normalize_weights(image_dataset_t* dataset);
void build_cascade(image_dataset_t* dataset, double max_false_positive_rate, double min_detection_rate);
int adaboost(adaboost_t* s_adaboost, image_dataset_t* dataset, cascade_t* cascade, int t);
int weak_classify(const features_array_t* array, int feature, Int64 threshold, int parity);
int strong_classify(const features_array_t* array, cascade_t* cascade);
