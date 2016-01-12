#include "adaboost.h"

int main() {
  init_sdl();
  image_dataset_t* dataset = alloc_image_dataset();
  dataset = fill_dataset(dataset);
  double max_false_positive_rate = 0.10;
  double min_detection_rate = 0.99;
  build_cascade(dataset, max_false_positive_rate, min_detection_rate);
  return 0;
}
