#include "adaboost.h"
#include <assert.h>
#include <SDL/SDL_thread.h>

#define MAX_CASCADE_LAYERS 10
static const int FEATURES[] = { 0, 2, 5, 10, 15, 20, 25, 30, 40, 50, 60 };

#if 0
#define DEBUG_BASE_LINE(...) printf(__VA_ARGS__)
#else
#define DEBUG_BASE_LINE(...) (void) 0
#endif

void compute_false_positive_rate(image_dataset_t* dataset, cascade_t* cascade) {
  int sum = 0;
  for (int i = 0; i < dataset->size_faces; ++i) {
    if (dataset->images[i] == NULL || !dataset->images[i]->consider) {
      continue;
    }
    int res = strong_classify(dataset->images[i]->features, cascade);
    sum += res;
  }
  cascade->detection_rate = (double) sum / dataset->size_faces;
  printf("faces:\n");
  printf("%d/%d detection_rate=%lf\n", sum, dataset->size_faces,
         cascade->detection_rate);

  sum = 0;
  for (int i = dataset->size_faces; i < dataset->size; ++i) {
    if (dataset->images[i] == NULL || !dataset->images[i]->consider) {
      continue;
    }
    int res = strong_classify(dataset->images[i]->features, cascade);
    sum += res;
  }
  cascade->false_positive_rate = (double) sum / dataset->size_non_faces;
  printf("non faces:\n");
  printf("%d/%d false_positive_rate=%lf\n", sum, dataset->size_non_faces, cascade->false_positive_rate);
}

void build_cascade(image_dataset_t* dataset, double max_false_positive_rate, double min_detection_rate) {
  FILE *fp = fopen("adaboost.txt", "w");
  if (fp == NULL) {
    printf("failed open adaboost.txt");
    exit(-1);
  }

  double false_positive_target = 0.00001;
  (void) false_positive_target;
  double current_false_positive_rate = 1.0;
  double previous_false_positive_rate = 1.0;
  double current_detection_rate = 1.0;
  double previous_detection_rate = 1.0;
  (void) max_false_positive_rate;
  (void) previous_detection_rate;
  (void) min_detection_rate;
  int i = 0;
  //compute number of classifiers
  while (i < MAX_CASCADE_LAYERS /* && current_false_positive_rate > false_positive_target */) {
    ++i;
    printf("\nlayer %d size_non_faces %d\n", i, dataset->size_non_faces);

    int nb_features = 0;
    current_false_positive_rate = previous_false_positive_rate;
    adaboost_t* s_adaboost = alloc_adaboost(162336);

    double optimal_false_positive_rate = current_false_positive_rate;
    double optimal_detection_rate = current_detection_rate;
    cascade_t* cascade = alloc_cascade();

    //compute number of features in the i-th classifier
    //while (optimal_false_positive_rate > max_false_positive_rate * previous_false_positive_rate) {
    //++nb_features;
    for (nb_features = 1; nb_features <=FEATURES[i]; ++nb_features) {
      reinit_cascade(cascade, nb_features);
      adaboost(s_adaboost, dataset, cascade, nb_features - 1);

      optimal_false_positive_rate = current_false_positive_rate * cascade->false_positive_rate;
      optimal_detection_rate = current_detection_rate * cascade->detection_rate;

      /* while (optimal_detection_rate < min_detection_rate * previous_detection_rate) { */
      /*   cascade->strong_threshold *= 0.95; */
      /*   compute_false_positive_rate(dataset, cascade); */
      /*   printf("    decreasing threshold %lf\n", cascade->strong_threshold); */
      /*   /\* printf("    detection_rate=%lf < fxFi-1=%lf\n", *\/ */
      /*   /\*        cascade->detection_rate, min_detection_rate * previous_detection_rate); *\/ */
      /*   optimal_false_positive_rate = current_false_positive_rate * cascade->false_positive_rate; */
      /*   optimal_detection_rate = current_detection_rate * cascade->detection_rate; */
      /* } */
      //}
    }

    current_false_positive_rate = optimal_false_positive_rate;
    current_detection_rate = optimal_detection_rate;

    printf("\nend layer %d current_false_positive_rate %lf current_detection_rate %lf\n", i,
           current_false_positive_rate, current_detection_rate);

    free_adaboost(s_adaboost);

    //write in adaboost.txt
    fprintf(fp, "layer=%d strong_threshold=%lf nb_features=%d\n",
            i, cascade->strong_threshold, nb_features - 1);
    for (int k = 0; k < nb_features - 1; ++k) {
      feature_t f = dataset->images[0]->features->features[cascade->features[k]];
      fprintf(fp, "%d feature=%d alpha=%lf threshold=%lli parity=%d j=%d i=%d w=%d h=%d charac=%d\n", k,
              cascade->features[k], cascade->alphas[k], cascade->thresholds[k], cascade->parities[k],
              f.j, f.i, f.w, f.h, f.charac);
    }
    fflush(fp);

    //remove true negative images
    //if (current_false_positive_rate > false_positive_target) {
      /* for (int k = dataset->size_faces; k < dataset->size; ++k) { */
      /*   if (dataset->images[k] == NULL) { */
      /*     continue; */
      /*   } */
      /*   if (!strong_classify(dataset->images[k]->features, cascade)) { */
      /*     free_training_image(dataset->images[k]); */
      /*     dataset->images[k] = NULL; */
      /*     --(dataset->size_non_faces); */
      /*   } */
      /* } */
      dataset->size_non_faces = 0;
      for (int k = dataset->size_faces; k < dataset->size; ++k) {
        if (dataset->images[k] == NULL) {
          continue;
        }
        if (!dataset->images[k]->is_face) {
          if (strong_classify(dataset->images[k]->features, cascade)) {
            ++(dataset->size_non_faces);
            dataset->images[k]->consider = 1;
          } else {
            dataset->images[k]->consider = 0;
          }
        }
      }
      //}

    previous_false_positive_rate = current_false_positive_rate;
    previous_detection_rate = current_detection_rate;

    free_cascade(cascade);
  }
  fclose(fp);
}

typedef struct {
  int lower_feature;
  int upper_feature;
  const image_dataset_t* dataset;
  adaboost_t* adaboost;
} thread_adaboost_train_args_t;

int start_thread_adaboost(void* arg) {
  thread_adaboost_train_args_t* args = (thread_adaboost_train_args_t*) arg;
  for (int i = args->lower_feature; i < args->upper_feature; ++i) {
    adaboost_train_feature(args->adaboost, i, args->dataset);
  }
  free(args);
  return 0;
}

SDL_Thread* thread_adaboost_train_feature(adaboost_t* s_adaboost, int lower_feature, int upper_feature, const image_dataset_t* dataset) {
  thread_adaboost_train_args_t *args  = malloc(sizeof(thread_adaboost_train_args_t));
  args->lower_feature = lower_feature;
  args->upper_feature = upper_feature;
  args->dataset = dataset;
  args->adaboost = s_adaboost;
  return SDL_CreateThread(&start_thread_adaboost, args);
}

int adaboost(adaboost_t* s_adaboost, image_dataset_t* dataset, cascade_t* cascade, int t) {
  //strong_classifier will be a linear combination of T weak classifiers
  printf("\nt=%d\n", t);
  //normalize the weights
  if (t == 0) {
   for (int w = 0; w < dataset->size; ++w) {
      if (dataset->images[w] == NULL || !dataset->images[w]->consider) {
        continue;
      }
      if (dataset->images[w]->is_face) {
        dataset->images[w]->weight = 1.0 / (2 * dataset->size_faces);
        } else {
        dataset->images[w]->weight = 1.0 / (2 * dataset->size_non_faces);
      }
    }
  } else {
    normalize_weights(dataset);
  }

  const int N_THREADS = 4;
  SDL_Thread* threads[N_THREADS];
  const int features_per_thread = 162336 / N_THREADS;
  for (int i = 0; i < N_THREADS; ++i) {
    int lower_feature = i * features_per_thread;
    int upper_feature = i == N_THREADS - 1 ? 162336 : (i + 1) * features_per_thread;
    threads[i] = thread_adaboost_train_feature(s_adaboost, lower_feature, upper_feature, dataset);
  }
  for (int i = 0; i < N_THREADS; ++i) {
    SDL_WaitThread(threads[i], NULL);
  }
  //for each feature j, we train a classifier
  /* for (int j = 0; j < s_adaboost->size; ++j) { */
  /*   adaboost_train_feature(s_adaboost, j, dataset); */
  /* } */

  //we choose the weak classifier with the lowest error
  double error_min = s_adaboost->errors[0];
  int feature = 0;
  for (int i = 1; i < s_adaboost->size; ++i) {
    //printf("feature %d error %lf error_min %lf\n", i, s_adaboost->errors[i], error_min);
    if (s_adaboost->errors[i] < error_min) {
      error_min = s_adaboost->errors[i];
      feature = i;
    }
  }

  cascade->parities[t] = s_adaboost->parities[feature];
  cascade->thresholds[t] = s_adaboost->thresholds[feature];
  cascade->features[t] = feature;
  cascade->alphas[t] = s_adaboost->alphas[feature];

  printf("feature selected=%d image=%p error_min=%lf threshold=%lli alpha=%lf parity=%d\n",
         feature, (void*) s_adaboost->images[feature], error_min, cascade->thresholds[t],
         cascade->alphas[t], cascade->parities[t]);

  //update the weights
  for (int i = 0; i < dataset->size; ++i) {
    if (dataset->images[i] == NULL || !dataset->images[i]->consider) {
      continue;
    }
    if (dataset->images[i]->is_face == weak_classify(dataset->images[i]->features, feature,
                                                     s_adaboost->thresholds[feature],
                                                     s_adaboost->parities[feature])) {
      dataset->images[i]->weight *= (error_min / (1.0 - error_min));
    }
  }

  //compute strong threshold by summing alphas
  cascade->strong_threshold = 0;
  for (int i = 0; i < cascade->nb_features; ++i) {
    cascade->strong_threshold += cascade->alphas[i];
  }
  cascade->strong_threshold /= 2;

  //compute false positive rate and detection rate
  compute_false_positive_rate(dataset, cascade);

  return 0;
}

int strong_classify(const features_array_t* array, cascade_t* cascade) {
  double sum = 0;
  for (int t = 0; t < cascade->nb_features; ++t) {
    int h = weak_classify(array, cascade->features[t], cascade->thresholds[t], cascade->parities[t]);
    sum += cascade->alphas[t] * h;
  }
  assert(!isnan(sum));
  DEBUG_BASE_LINE("sum %lf strong_threshold %lf\n", sum, cascade->strong_threshold);
  return sum >= cascade->strong_threshold ? 1 : 0;
}

int weak_classify(const features_array_t* array, int feature, Int64 threshold, int parity) {
  Int64 value = array->features[feature].value;
  if (parity * value < parity * threshold) {
      return 1;
  }
  return 0;
}

void normalize_weights(image_dataset_t* dataset) {
  double sum = 0;
  for (int i = 0; i < dataset->size; ++i) {
    if (dataset->images[i] == NULL || !dataset->images[i]->consider) {
      continue;
    }
    sum += dataset->images[i]->weight;
  }
  for (int i = 0; i < dataset->size; ++i) {
    if (dataset->images[i] == NULL || !dataset->images[i]->consider) {
      continue;
    }
    dataset->images[i]->weight = dataset->images[i]->weight / sum;
  }
}

int sort_by_feature_value(const void* v1, const void* v2, void* arg) {
  int feature = *((int*) arg);
  const training_image_t* image1 = *((const training_image_t**) v1);
  const training_image_t* image2 = *((const training_image_t**) v2);
  if ((image1 == NULL || !image1->consider) && (image2 == NULL || !image2->consider)) {
    return 0;
  }
  if (image1 == NULL || !image1->consider) {
    return 1;
  }
  if (image2 == NULL || !image2->consider) {
    return -1;
  }
  if (image1->features->features[feature].value < image2->features->features[feature].value) {
    return -1;
  }
  if (image1->features->features[feature].value > image2->features->features[feature].value) {
    return 1;
  }
  return 0;
}

#define MIN(A, B) (A) < (B) ? (A) : (B);

void adaboost_train_feature(adaboost_t* adaboost, int feature, const image_dataset_t* dataset) {
  training_image_t** images = (training_image_t**) malloc(sizeof(training_image_t*) * dataset->size);
  memcpy(images, dataset->images, sizeof(training_image_t*) * dataset->size);
  qsort_r(images, dataset->size, sizeof(training_image_t*), &sort_by_feature_value, &feature);
  double Tp = 0;
  double Tn = 0;
  double Sp = 0;
  double Sn = 0;

  for (int i = 0; i < dataset->size; ++i) {
    if (images[i] == NULL || !dataset->images[i]->consider) {
      continue;
    }
    if (images[i]->is_face) {
      Tp += images[i]->weight;
    } else {
      Tn += images[i]->weight;
    }
  }

  double min_error = MIN(Sp + Tn - Sn, Sn + Tp - Sp);
  Int64 optimal_threshold = images[0]->features->features[feature].value;
  int parity = (Sn + Tp - Sp) < (Sp + Tn - Sn) ? 1 : -1;
  training_image_t* image = images[0];

  for (int i = 1; i < dataset->size; ++i) {
    if (images[i] == NULL || !dataset->images[i]->consider) {
      continue;
    }

    if (images[i]->is_face) {
      Sp += images[i]->weight;
    } else {
      Sn += images[i]->weight;
    }

    double error = MIN(Sp + Tn - Sn, Sn + Tp - Sp);

    if (error < min_error) {
      min_error = error;
      optimal_threshold = images[i]->features->features[feature].value;
      parity = (Sn + Tp - Sp) < (Sp + Tn - Sn) ? 1 : -1;
      image = images[i];
    }
  }

  /* if (min_error < 0.00001) { */
  /*   printf("begin\n"); */
  /*   for (int i = 0; i < dataset->size; ++i) { */
  /*     if (images[i] == NULL) { */
  /*       continue; */
  /*     } */
  /*     if (images[i]->features->features[feature].value == optimal_threshold) { */
  /*       printf("| (%lli) ", optimal_threshold); */
  /*     } else { */
  /*       if (images[i]->is_face) { */
  /*         printf("+ "); */
  /*       } else { */
  /*         printf("- (%lf) ", images[i]->weight); */
  /*       } */
  /*     } */
  /*   } */
  /*   printf("\nend\n"); */
  /* } */

  adaboost->images[feature] = image;
  adaboost->parities[feature] = parity;
  adaboost->errors[feature] = min_error;
  adaboost->alphas[feature] = log((1.0 - min_error) / min_error);
  adaboost->thresholds[feature] = optimal_threshold;
  free(images);
}
