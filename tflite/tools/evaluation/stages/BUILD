# Copyright 2019 The TensorFlow Authors. All Rights Reserved.
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

load("//tflite:build_def.bzl", "tflite_copts", "tflite_linkopts")

package(
    # copybara:uncomment default_applicable_licenses = ["@org_tensorflow//tensorflow:license"],
    default_visibility = ["//visibility:public"],
    licenses = ["notice"],
)

common_linkopts = tflite_linkopts() + select({
    "//conditions:default": [],
    "@org_tensorflow//tensorflow:android": [
        "-pie",
        "-llog",
    ],
})

exports_files(glob([
    "testdata/*.jpg",
]))

cc_library(
    name = "image_preprocessing_stage",
    srcs = ["image_preprocessing_stage.cc"],
    hdrs = ["image_preprocessing_stage.h"],
    copts = tflite_copts(),
    deps = [
        "//tflite:string",
        "//tflite/c:c_api_types",
        "//tflite/kernels/internal:reference_base",
        "//tflite/kernels/internal:types",
        "//tflite/profiling:time",
        "//tflite/tools/evaluation:evaluation_stage",
        "//tflite/tools/evaluation/proto:evaluation_config_cc_proto",
        "//tflite/tools/evaluation/proto:evaluation_stages_cc_proto",
        "//tflite/tools/evaluation/proto:preprocessing_steps_cc_proto",
        "@com_google_absl//absl/base",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
        "@libjpeg_turbo//:jpeg",
        "@local_xla//xla/tsl/util:stats_calculator_portable",
        "@org_tensorflow//tensorflow/core:tflite_portable_logging",
    ] + select({
        "@org_tensorflow//tensorflow:android": [
            "@org_tensorflow//tensorflow/core:portable_jpeg_internal",
        ],
        "//conditions:default": [
            "@org_tensorflow//tensorflow/core:jpeg_internal",
        ],
    }),
)

cc_test(
    name = "image_preprocessing_stage_test",
    srcs = ["image_preprocessing_stage_test.cc"],
    data = ["testdata/grace_hopper.jpg"],
    linkopts = common_linkopts,
    linkstatic = 1,
    deps = [
        ":image_preprocessing_stage",
        "//tflite/c:c_api_types",
        "//tflite/tools/evaluation/proto:evaluation_config_cc_proto",
        "//tflite/tools/evaluation/proto:evaluation_stages_cc_proto",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "topk_accuracy_eval_stage",
    srcs = ["topk_accuracy_eval_stage.cc"],
    hdrs = ["topk_accuracy_eval_stage.h"],
    copts = tflite_copts(),
    deps = [
        "//tflite/c:c_api_types",
        "//tflite/c:common",
        "//tflite/tools/evaluation:evaluation_stage",
        "//tflite/tools/evaluation/proto:evaluation_config_cc_proto",
        "//tflite/tools/evaluation/proto:evaluation_stages_cc_proto",
        "@com_google_absl//absl/log",
        "@org_tensorflow//tensorflow/core:tflite_portable_logging",
    ],
)

cc_test(
    name = "topk_accuracy_eval_stage_test",
    srcs = ["topk_accuracy_eval_stage_test.cc"],
    linkopts = common_linkopts,
    linkstatic = 1,
    deps = [
        ":topk_accuracy_eval_stage",
        "//tflite/c:c_api_types",
        "//tflite/c:common",
        "//tflite/tools/evaluation/proto:evaluation_config_cc_proto",
        "//tflite/tools/evaluation/proto:evaluation_stages_cc_proto",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "tflite_inference_stage",
    srcs = ["tflite_inference_stage.cc"],
    hdrs = ["tflite_inference_stage.h"],
    copts = tflite_copts(),
    deps = [
        "//tflite:framework",
        "//tflite/core:framework",
        "//tflite/core/c:common",
        "//tflite/core/kernels:builtin_ops",
        "//tflite/profiling:time",
        "//tflite/tools/evaluation:evaluation_delegate_provider",
        "//tflite/tools/evaluation:evaluation_stage",
        "//tflite/tools/evaluation:utils",
        "//tflite/tools/evaluation/proto:evaluation_config_cc_proto",
        "//tflite/tools/evaluation/proto:evaluation_stages_cc_proto",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/log",
        "@local_xla//xla/tsl/util:stats_calculator_portable",
        "@org_tensorflow//tensorflow/core:tflite_portable_logging",
    ],
)

cc_test(
    name = "tflite_inference_stage_test",
    srcs = ["tflite_inference_stage_test.cc"],
    data = ["//tflite:testdata/add_quantized.bin"],
    linkopts = common_linkopts,
    linkstatic = 1,
    deps = [
        ":tflite_inference_stage",
        "//tflite:framework",
        "//tflite/core:framework",
        "//tflite/core/c:common",
        "//tflite/tools/evaluation:utils",
        "//tflite/tools/evaluation/proto:evaluation_config_cc_proto",
        "//tflite/tools/evaluation/proto:evaluation_stages_cc_proto",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "image_classification_stage",
    srcs = ["image_classification_stage.cc"],
    hdrs = ["image_classification_stage.h"],
    copts = tflite_copts(),
    deps = [
        ":image_preprocessing_stage",
        ":tflite_inference_stage",
        ":topk_accuracy_eval_stage",
        "//tflite/c:c_api_types",
        "//tflite/tools/evaluation:evaluation_delegate_provider",
        "//tflite/tools/evaluation:evaluation_stage",
        "//tflite/tools/evaluation:utils",
        "//tflite/tools/evaluation/proto:evaluation_config_cc_proto",
        "//tflite/tools/evaluation/proto:evaluation_stages_cc_proto",
        "@com_google_absl//absl/log",
        "@org_tensorflow//tensorflow/core:tflite_portable_logging",
    ],
)

cc_library(
    name = "inference_profiler_stage",
    srcs = ["inference_profiler_stage.cc"],
    hdrs = ["inference_profiler_stage.h"],
    copts = tflite_copts(),
    deps = [
        ":tflite_inference_stage",
        "//tflite/c:c_api_types",
        "//tflite/tools/evaluation:evaluation_delegate_provider",
        "//tflite/tools/evaluation:evaluation_stage",
        "//tflite/tools/evaluation/proto:evaluation_config_cc_proto",
        "//tflite/tools/evaluation/proto:evaluation_stages_cc_proto",
        "@FP16",
        "@com_google_absl//absl/log",
        "@local_xla//xla/tsl/util:stats_calculator_portable",
        "@org_tensorflow//tensorflow/core:tflite_portable_logging",
    ],
)

cc_test(
    name = "inference_profiler_stage_test",
    srcs = ["inference_profiler_stage_test.cc"],
    data = ["//tflite:testdata/add_quantized.bin"],
    linkopts = common_linkopts,
    linkstatic = 1,
    deps = [
        ":inference_profiler_stage",
        "//tflite/core/c:common",
        "//tflite/delegates/nnapi:nnapi_delegate",
        "//tflite/tools/evaluation/proto:evaluation_config_cc_proto",
        "//tflite/tools/evaluation/proto:evaluation_stages_cc_proto",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "object_detection_average_precision_stage",
    srcs = ["object_detection_average_precision_stage.cc"],
    hdrs = ["object_detection_average_precision_stage.h"],
    copts = tflite_copts(),
    deps = [
        "//tflite/c:c_api_types",
        "//tflite/core/c:common",
        "//tflite/tools/evaluation:evaluation_stage",
        "//tflite/tools/evaluation/proto:evaluation_config_cc_proto",
        "//tflite/tools/evaluation/proto:evaluation_stages_cc_proto",
        "//tflite/tools/evaluation/stages/utils:image_metrics",
        "@com_google_absl//absl/log",
        "@org_tensorflow//tensorflow/core:tflite_portable_logging",
    ],
)

cc_test(
    name = "object_detection_average_precision_stage_test",
    srcs = ["object_detection_average_precision_stage_test.cc"],
    linkopts = common_linkopts,
    linkstatic = 1,
    deps = [
        ":object_detection_average_precision_stage",
        "//tflite/c:c_api_types",
        "//tflite/tools/evaluation/proto:evaluation_config_cc_proto",
        "//tflite/tools/evaluation/proto:evaluation_stages_cc_proto",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "object_detection_stage",
    srcs = ["object_detection_stage.cc"],
    hdrs = ["object_detection_stage.h"],
    copts = tflite_copts(),
    deps = [
        ":image_preprocessing_stage",
        ":object_detection_average_precision_stage",
        ":tflite_inference_stage",
        "//tflite/c:c_api_types",
        "//tflite/core/c:common",
        "//tflite/tools/evaluation:evaluation_delegate_provider",
        "//tflite/tools/evaluation:evaluation_stage",
        "//tflite/tools/evaluation:utils",
        "//tflite/tools/evaluation/proto:evaluation_config_cc_proto",
        "//tflite/tools/evaluation/proto:evaluation_stages_cc_proto",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log",
        "@org_tensorflow//tensorflow/core:tflite_portable_logging",
    ],
)
