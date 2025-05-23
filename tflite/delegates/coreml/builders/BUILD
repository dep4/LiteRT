# Copyright 2020 The TensorFlow Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==============================================================================

package(
    default_visibility = [
        "//visibility:public",
    ],
    licenses = ["notice"],  # Apache 2.0
)

cc_library(
    name = "op_builder",
    srcs = glob(["*_builder.cc"]),
    hdrs = glob(["*_builder.h"]),
    deps = [
        ":op_factory",
        ":op_validator",
        ":util",
        "//tflite:kernel_api",
        "//tflite/core/c:common",
        "//tflite/kernels:kernel_util",
        "//tflite/kernels/internal:optimized_base",
        "//tflite/kernels/internal:tensor",
        "//tflite/kernels/internal:types",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_protobuf//:protobuf_headers",
        "@coremltools//:mlmodel_cc_proto",
    ],
)

cc_library(
    name = "op_factory",
    hdrs = ["op_factory.h"],
    deps = [
        "//tflite/core/c:common",
    ],
)

cc_library(
    name = "op_validator",
    hdrs = ["op_validator.h"],
    deps = [
        "//tflite/core/c:common",
    ],
)

cc_library(
    name = "util",
    srcs = ["util.cc"],
    hdrs = ["util.h"],
    deps = [
        ":op_validator",
        "//tflite/core/c:common",
        "//tflite/kernels:kernel_util",
        "//tflite/kernels/internal:tensor",
        "@FP16",
    ],
)

cc_test(
    name = "util_test",
    srcs = ["util_test.cc"],
    deps = [
        ":util",
        "//tflite/core/c:common",
        "@com_google_googletest//:gtest_main",
        "@eigen_archive//:eigen3",
    ],
)
