/* Copyright 2023 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#ifndef TENSORFLOW_LITE_EXPERIMENTAL_ML_ADJACENT_ALGO_RGB_TO_GRAYSCALE_H_
#define TENSORFLOW_LITE_EXPERIMENTAL_ML_ADJACENT_ALGO_RGB_TO_GRAYSCALE_H_

#include "tflite/experimental/ml_adjacent/lib.h"

namespace ml_adj {
namespace rgb_to_grayscale {

// RGB to Grayscale conversion
//
// Inputs: [img: float]
// Ouputs: [img: float]
//
// Converts one or more images from RGB to Grayscale.
// Mimics semantic of `tf.image.rgb_to_grayscale`.

// https://www.tensorflow.org/api_docs/python/tf/image/rgb_to_grayscale

const algo::Algo* Impl_RgbToGrayscale();

}  // namespace rgb_to_grayscale
}  // namespace ml_adj

#endif  // TENSORFLOW_LITE_EXPERIMENTAL_ML_ADJACENT_ALGO_RGB_TO_GRAYSCALE_H_
