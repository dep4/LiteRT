/* Copyright 2021 The TensorFlow Authors. All Rights Reserved.

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

#include "tflite/kernels/gradient/gradient_ops.h"

#include "tflite/kernels/gradient/bcast_grad_args.h"
#include "tflite/mutable_op_resolver.h"

namespace tflite {
namespace ops {
namespace custom {

extern "C" void AddGradientOps(::tflite::MutableOpResolver* resolver) {
  resolver->AddCustom("BroadcastGradientArgs",
                      tflite::ops::custom::Register_BROADCAST_GRADIENT_ARGS());
}

}  // namespace custom
}  // namespace ops
}  // namespace tflite
