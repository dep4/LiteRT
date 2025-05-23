# Copyright (c) Qualcomm Innovation Center, Inc. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

package(
    # copybara:uncomment default_applicable_licenses = ["@org_tensorflow//tensorflow:license"],
    default_visibility = ["//tflite/experimental/litert/vendors/qualcomm:__subpackages__"],
)

cc_library(
    name = "quantize_params_wrapper",
    srcs = ["quantize_params_wrapper.cc"],
    hdrs = ["quantize_params_wrapper.h"],
    tags = [
        # Don't build/test in OS until qnn is available.
        "nobuilder",
    ],
    deps = [
        "@com_google_absl//absl/types:span",
        # copybara:uncomment "@org_tensorflow//third_party/qairt/latest:qnn_lib_headers",
    ],
)

cc_library(
    name = "tensor_wrapper",
    srcs = ["tensor_wrapper.cc"],
    hdrs = ["tensor_wrapper.h"],
    tags = [
        # Don't build/test in OS until qnn is available.
        "nobuilder",
    ],
    deps = [
        ":quantize_params_wrapper",
        "@com_google_absl//absl/types:span",
        # copybara:uncomment "@org_tensorflow//third_party/qairt/latest:qnn_lib_headers",
        "//tflite/experimental/litert/vendors/qualcomm/core/utils:log",
        "//tflite/experimental/litert/vendors/qualcomm/core/utils:miscs",
    ],
)

cc_library(
    name = "param_wrapper",
    srcs = ["param_wrapper.cc"],
    hdrs = ["param_wrapper.h"],
    tags = [
        # Don't build/test in OS until qnn is available.
        "nobuilder",
    ],
    deps = [
        # copybara:uncomment "@org_tensorflow//third_party/qairt/latest:qnn_lib_headers",
        "//tflite/experimental/litert/vendors/qualcomm/core/utils:log",
        "//tflite/experimental/litert/vendors/qualcomm/core/utils:miscs",
        "//tflite/experimental/litert/vendors/qualcomm/core/wrappers:tensor_wrapper",
    ],
)

cc_library(
    name = "op_wrapper",
    srcs = ["op_wrapper.cc"],
    hdrs = ["op_wrapper.h"],
    tags = [
        # Don't build/test in OS until qnn is available.
        "nobuilder",
    ],
    deps = [
        # copybara:uncomment "@org_tensorflow//third_party/qairt/latest:qnn_lib_headers",
        "//tflite/experimental/litert/vendors/qualcomm/core/wrappers:param_wrapper",
        "//tflite/experimental/litert/vendors/qualcomm/core/wrappers:tensor_wrapper",
    ],
)
