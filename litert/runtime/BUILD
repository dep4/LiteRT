# Copyright 2024 Google LLC.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

load("//litert/build_common:litert_build_defs.bzl", "gtest_main_no_heapcheck_deps")
load("//litert/build_common:special_rule.bzl", "gles_deps", "gles_linkopts", "lite_rt_friends")

package(
    # copybara:uncomment default_applicable_licenses = ["@org_tensorflow//tensorflow:license"],
    default_visibility = ["//litert:__subpackages__"],
)

cc_library(
    name = "event",
    srcs = [
        "event.cc",
    ],
    hdrs = [
        "event.h",
    ],
    deps = [
        "//litert/c:litert_common",
        "//litert/cc:litert_expected",
    ],
)

cc_library(
    name = "tensor_buffer",
    srcs = [
        "ahwb_buffer.cc",
        "dmabuf_buffer.cc",
        "fastrpc_buffer.cc",
        "gl_buffer.cc",
        "gl_texture.cc",
        "ion_buffer.cc",
        "open_cl_buffer.cc",
        "tensor_buffer.cc",
    ],
    hdrs = [
        "ahwb_buffer.h",
        "dmabuf_buffer.h",
        "event.h",
        "fastrpc_buffer.h",
        "gl_buffer.h",
        "gl_texture.h",
        "ion_buffer.h",
        "open_cl_buffer.h",
        "tensor_buffer.h",
        "tensor_buffer_requirements.h",
        "//litert/c:litert_event.h",
        "//litert/c:litert_tensor_buffer.h",
        "//litert/c:litert_tensor_buffer_requirements.h",
    ],
    linkopts = gles_linkopts(),
    deps = [
        ":environment",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:node_hash_map",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/types:span",
        "//litert/c:litert_common",
        "//litert/c:litert_layout",
        "//litert/c:litert_model",
        "//litert/c:litert_tensor_buffer_types",
        "//litert/cc:litert_event",
        "//litert/cc:litert_expected",
        "//litert/cc:litert_macros",
        "//litert/cc:litert_tensor_buffer_utils",
        "//litert/core/util:tensor_type_util",
        "//litert/runtime/opencl:buffer",
        "//litert/runtime/opencl:cl_command_queue",
        "//litert/runtime/opencl:cl_context",
        "//litert/runtime/opencl:opencl_wrapper",
        "@opencl_headers",
    ] + gles_deps() + select({
        "@org_tensorflow//tensorflow:android": [
            "@org_tensorflow//tensorflow/lite/delegates/gpu/gl:gl_buffer",
            "@org_tensorflow//tensorflow/lite/delegates/gpu/gl:gl_texture",
        ],
        "//conditions:default": [],
    }),
)

cc_library(
    name = "environment",
    srcs = [
        "environment.cc",
    ],
    hdrs = [
        "environment.h",
    ],
    visibility = [
        "//third_party/odml/infra/ml_drift_delegate:__subpackages__",
    ],
    deps = [
        "//litert/c:litert_any",
        "//litert/c:litert_common",
        "//litert/c:litert_logging",
        "//litert/cc:litert_expected",
        "//litert/core:environment",
        "//litert/runtime/opencl:cl_command_queue",
        "//litert/runtime/opencl:cl_context",
        "//litert/runtime/opencl:cl_device",
        "//litert/runtime/opencl:opencl_wrapper",
        "@opencl_headers",
    ],
)

cc_test(
    name = "environment_test",
    srcs = ["environment_test.cc"],
    linkopts = select({
        "@org_tensorflow//tensorflow:android": ["-landroid"],
        "//conditions:default": [],
    }),
    tags = [
        "requires-gpu-nvidia",
    ],
    deps = [
        ":environment",
        "@com_google_googletest//:gtest_main",
        # copybara:uncomment_begin(google-only)
        # "@ml_drift//ml_drift/cl:environment",
        # "@ml_drift//ml_drift/cl:opencl_wrapper",
        # copybara:uncomment_end
        "//litert/c:litert_environment",
        "//litert/cc:litert_any",
        "//litert/runtime/opencl:opencl_wrapper",
    ],
)

cc_library(
    name = "tfl_utils",
    srcs = [
        "tfl_utils.cc",
    ],
    hdrs = [
        "tfl_utils.h",
    ],
    deps = [
        "//litert/c:litert_common",
        "//litert/cc:litert_detail",
        "//litert/cc:litert_element_type",
        "//litert/cc:litert_expected",
        "//litert/cc:litert_model",
        "@org_tensorflow//tensorflow/lite/c:c_api",
        "@org_tensorflow//tensorflow/lite/c:c_api_opaque",
        "@org_tensorflow//tensorflow/lite/c:c_api_types",
        "@org_tensorflow//tensorflow/lite/c:common",
    ],
)

cc_library(
    name = "external_litert_buffer_context",
    srcs = ["external_litert_buffer_context.cc"],
    hdrs = ["external_litert_buffer_context.h"],
    visibility = [
        "//litert:__subpackages__",
    ] + lite_rt_friends(),
    deps = [
        ":tfl_utils",
        "@com_google_absl//absl/container:flat_hash_map",
        "//litert/c:litert_common",
        "//litert/c:litert_logging",
        "//litert/c:litert_model",
        "//litert/c:litert_tensor_buffer",
        "//litert/cc:litert_expected",
        "//litert/cc:litert_tensor_buffer",
        "//litert/cc:litert_tensor_buffer_requirements",
        "@org_tensorflow//tensorflow/lite/c:c_api",
        "@org_tensorflow//tensorflow/lite/c:c_api_opaque",
        "@org_tensorflow//tensorflow/lite/c:c_api_types",
        "@org_tensorflow//tensorflow/lite/c:common",
    ],
)

cc_library(
    name = "compiled_model",
    srcs = ["compiled_model.cc"],
    hdrs = ["compiled_model.h"],
    deps = [
        ":compilation_options",
        ":external_litert_buffer_context",
        ":tensor_buffer",
        "@com_google_absl//absl/cleanup",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:string_view",
        "//litert/c:litert_accelerator_options",
        "//litert/c:litert_common",
        "//litert/c:litert_compilation_options",
        "//litert/c:litert_dispatch_delegate",
        "//litert/c:litert_logging",
        "//litert/c:litert_model",
        "//litert/c:litert_tensor_buffer",
        "//litert/cc:litert_buffer_ref",
        "//litert/cc:litert_event",
        "//litert/cc:litert_expected",
        "//litert/cc:litert_macros",
        "//litert/cc:litert_model",
        "//litert/cc:litert_tensor_buffer",
        "//litert/cc:litert_tensor_buffer_requirements",
        "//litert/compiler/plugin:compiler_plugin",
        "//litert/core:accelerator",
        "//litert/core:accelerator_model_compilation_data",
        "//litert/core:build_stamp",
        "//litert/core:environment",
        "//litert/core/model",
        "//litert/runtime/dispatch:dispatch_delegate",
        "@org_tensorflow//tensorflow/compiler/mlir/lite:allocation",
        "@org_tensorflow//tensorflow/lite:builtin_ops",
        "@org_tensorflow//tensorflow/lite:framework",
        "@org_tensorflow//tensorflow/lite:model_builder",
        "@org_tensorflow//tensorflow/lite/c:c_api_opaque",
        "@org_tensorflow//tensorflow/lite/c:common",
        "@org_tensorflow//tensorflow/lite/core:cc_api_stable",
        "@org_tensorflow//tensorflow/lite/delegates/utils:simple_opaque_delegate",
        "@org_tensorflow//tensorflow/lite/kernels:builtin_ops",
    ],
)

cc_test(
    name = "compiled_model_test",
    srcs = ["compiled_model_test.cc"],
    data = [
        "//litert/test:testdata/simple_model.tflite",
    ],
    linkopts = select({
        "@org_tensorflow//tensorflow:android": ["-landroid"],
        "//conditions:default": [],
    }),
    # require GPU to run OpenCL tests.
    tags = [
        "requires-gpu-nvidia",
    ],
    deps = [
        ":compiled_model",
        ":tensor_buffer",
        "@com_google_googletest//:gtest_main",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/strings:string_view",
        "@com_google_absl//absl/types:span",
        "//litert/c:litert_common",
        "//litert/c:litert_compilation_options",
        "//litert/c:litert_environment",
        "//litert/c:litert_model",
        "//litert/c:litert_tensor_buffer",
        "//litert/cc:litert_expected",
        "//litert/cc:litert_macros",
        "//litert/cc:litert_tensor_buffer",
        "//litert/core/model",
        "//litert/test:common",
        "//litert/test:matchers",
        "//litert/test:simple_model",
        "@org_tensorflow//tensorflow/lite:framework",
        "@org_tensorflow//tensorflow/lite/c:c_api_opaque",
        "@org_tensorflow//tensorflow/lite/c:common",
        "@org_tensorflow//tensorflow/lite/kernels:builtin_ops",
    ],
)

cc_library(
    name = "compilation_options",
    hdrs = [
        "compilation_options.h",
        "//litert/c:litert_compilation_options.h",
    ],
    deps = [
        "//litert/c:litert_accelerator_options",
        "//litert/c:litert_common",
        "//litert/c:litert_layout",
        "//litert/c:litert_tensor_buffer",
    ],
)

cc_test(
    name = "gl_buffer_test",
    srcs = ["gl_buffer_test.cc"],
    linkopts = select({
        "@org_tensorflow//tensorflow:android": [
            "-landroid",
        ],
        "//conditions:default": [],
    }),
    tags = [
        "notap",
    ],
    deps = [
        ":tensor_buffer",
        "//litert/c:litert_common",
        "//litert/test:matchers",
    ] + gtest_main_no_heapcheck_deps() + select({
        "@org_tensorflow//tensorflow:android": [
            "@org_tensorflow//tensorflow/lite/delegates/gpu/gl:egl_environment",
            "@org_tensorflow//tensorflow/lite/delegates/gpu/gl:gl_buffer",
        ],
        "//conditions:default": [],
    }),
)

cc_library(
    name = "tensor_buffer_conversion",
    srcs = ["tensor_buffer_conversion.cc"],
    hdrs = ["tensor_buffer_conversion.h"],
    linkopts = gles_linkopts(),
    deps = [
        ":tensor_buffer",
        "@com_google_absl//absl/strings:str_format",
        "//litert/c:litert_common",
        "//litert/c:litert_environment",
        "//litert/c:litert_tensor_buffer_types",
        "//litert/cc:litert_expected",
        "//litert/cc:litert_macros",
        "//litert/cc:litert_tensor_buffer_utils",
    ] + gles_deps(),
)

cc_test(
    name = "tensor_buffer_conversion_test",
    srcs = ["tensor_buffer_conversion_test.cc"],
    linkopts = select({
        "@org_tensorflow//tensorflow:android": ["-landroid"],
        "//conditions:default": [],
    }),
    tags = [
        "notap",
    ],
    deps = [
        ":tensor_buffer",
        ":tensor_buffer_conversion",
        "@com_google_googletest//:gtest_main",
        "//litert/c:litert_common",
        "//litert/c:litert_environment",
        "//litert/c:litert_model",
        "//litert/c:litert_tensor_buffer_types",
        "//litert/cc:litert_layout",
        "//litert/core:environment",
        "//litert/test:matchers",
    ] + select({
        "@org_tensorflow//tensorflow:android": [
            "@org_tensorflow//tensorflow/lite/delegates/gpu/gl:egl_environment",
        ],
        "//conditions:default": [],
    }),
)
