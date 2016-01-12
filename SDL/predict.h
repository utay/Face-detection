#include "adaboost.h"
#include <assert.h>


void scan_image(SDL_Surface* image);
void print_features();
void test_features(int feature) ;
predict_cascade_t* load_adaboost(const char* file);