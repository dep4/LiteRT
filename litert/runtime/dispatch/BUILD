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

load("//litert/build_common:special_rule.bzl", "gles_linkopts")

package(
    # copybara:uncomment default_applicable_licenses = ["@org_tensorflow//tensorflow:license"],
    default_visibility = [
        "//third_party/odml/infra/perf/mobile_tests/litert:__subpackages__",
        "//litert:__subpackages__",
    ],
)

# Dispatch API implementation, it is used by the dispatch delegate to call the vendor's dispatch
# API.
cc_library(
    name = "dispatch",
    srcs = [
        "litert_dispatch.cc",
    ],
    hdrs = [
        "//litert/vendors/c:litert_dispatch.h",
        "//litert/vendors/c:litert_dispatch_api.h",
    ],
    deps = [
        "//litert/c:litert_any",
        "//litert/c:litert_common",
        "//litert/c:litert_event",
        "//litert/c:litert_logging",
        "//litert/c:litert_model",
        "//litert/c:litert_tensor_buffer",
        "//litert/cc:litert_expected",
        "//litert/core:dynamic_loading",
    ],
)

cc_library(
    name = "dispatch_delegate",
    srcs = [
        "dispatch_delegate.cc",
        "dispatch_delegate_kernel.cc",
    ],
    hdrs = [
        "dispatch_delegate_kernel.h",
        "dispatch_delegate_options.h",
        "//litert/c:litert_dispatch_delegate.h",
        "//litert/cc:litert_dispatch_delegate.h",
    ],
    deps = [
        ":dispatch",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/strings:string_view",
        "@com_google_absl//absl/types:span",
        "//litert/c:litert_any",
        "//litert/c:litert_common",
        "//litert/c:litert_event",
        "//litert/c:litert_logging",
        "//litert/c:litert_model",
        "//litert/c:litert_tensor_buffer",
        "//litert/cc:litert_any",
        "//litert/cc:litert_buffer_ref",
        "//litert/cc:litert_expected",
        "//litert/cc:litert_model",
        "//litert/cc:litert_tensor_buffer",
        "//litert/core:build_stamp",
        "//litert/core:dispatch_op_schema",
        "//litert/core:dynamic_loading",
        "//litert/core:environment",
        "//litert/runtime:external_litert_buffer_context",
        "//litert/runtime:tfl_utils",
        "//litert/vendors/c:litert_dispatch_c_api",
        "@org_tensorflow//tensorflow/lite/c:c_api",
        "@org_tensorflow//tensorflow/lite/c:c_api_opaque",
        "@org_tensorflow//tensorflow/lite/c:c_api_types",
        "@org_tensorflow//tensorflow/lite/c:common",
        "@org_tensorflow//tensorflow/lite/core/c:c_api_opaque_without_op_resolver",
        "@org_tensorflow//tensorflow/lite/delegates/utils:simple_opaque_delegate",
    ],
)

cc_test(
    name = "dispatch_delegate_google_tensor_test",
    srcs = ["dispatch_delegate_google_tensor_test.cc"],
    data = [
        "//litert/test:testdata/shared_input_cpu_npu.tflite",
        "//litert/vendors/google_tensor/dispatch:dispatch_api_so",
    ],
    linkopts = select({
        "@org_tensorflow//tensorflow:android": ["-landroid"],
        "//conditions:default": [],
    }) + gles_linkopts(),
    deps = [
        ":dispatch_delegate",
        "@com_google_googletest//:gtest_main",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/strings:string_view",
        "@com_google_absl//absl/types:span",
        # copybara:uncomment "//third_party/darwinn/driver_shared/fence:fence_test_util",
        "//litert/c:litert_runtime_c_api_shared_lib",
        "//litert/cc:litert_buffer_ref",
        "//litert/cc:litert_compilation_options",
        "//litert/cc:litert_compiled_model",
        "//litert/cc:litert_environment",
        "//litert/cc:litert_event",
        "//litert/cc:litert_expected",
        "//litert/cc:litert_model",
        "//litert/cc:litert_tensor_buffer",
        "//litert/core/model:model_buffer",
        "//litert/core/util:flatbuffer_tools",
        "//litert/runtime:external_litert_buffer_context",
        "//litert/test:common",
        "//litert/test:matchers",
        "//litert/test:simple_model_npu",
        "@org_tensorflow//tensorflow/lite:framework",
        "@org_tensorflow//tensorflow/lite/c:c_api_opaque",
    ],
)

cc_test(
    name = "dispatch_delegate_qualcomm_test",
    srcs = ["dispatch_delegate_qualcomm_test.cc"],
    data = [
        "//litert/test:testdata/shared_input_cpu_npu.tflite",
        "//litert/vendors/qualcomm/dispatch:dispatch_api_so",
    ],
    linkopts = select({
        "@org_tensorflow//tensorflow:android": ["-landroid"],
        "//conditions:default": [],
    }),
    deps = [
        ":dispatch_delegate",
        "@com_google_googletest//:gtest_main",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/strings:string_view",
        "@com_google_absl//absl/types:span",
        "//litert/c:litert_runtime_c_api_shared_lib",
        "//litert/cc:litert_compilation_options",
        "//litert/cc:litert_compiled_model",
        "//litert/cc:litert_dispatch_delegate",
        "//litert/cc:litert_environment",
        "//litert/cc:litert_model",
        "//litert/cc:litert_tensor_buffer",
        "//litert/runtime:external_litert_buffer_context",
        "//litert/test:common",
        "//litert/test:simple_model_npu",
        "@org_tensorflow//tensorflow/lite:framework",
        "@org_tensorflow//tensorflow/lite/c:c_api_opaque",
    ],
)

cc_test(
    name = "dispatch_delegate_mediatek_test",
    srcs = ["dispatch_delegate_mediatek_test.cc"],
    data = [
        "//litert/test:testdata/shared_input_cpu_npu.tflite",
        "//litert/vendors/mediatek/dispatch:dispatch_api_so",
    ],
    linkopts = select({
        "@org_tensorflow//tensorflow:android": ["-landroid"],
        "//conditions:default": [],
    }),
    tags = [
        "no_oss",
        "nobuilder",
        "notap",
    ],
    deps = [
        ":dispatch_delegate",
        "@com_google_googletest//:gtest_main",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/strings:string_view",
        "@com_google_absl//absl/types:span",
        "//litert/c:litert_runtime_c_api_shared_lib",
        "//litert/cc:litert_compilation_options",
        "//litert/cc:litert_compiled_model",
        "//litert/cc:litert_environment",
        "//litert/cc:litert_model",
        "//litert/cc:litert_tensor_buffer",
        "//litert/core/model:model_buffer",
        "//litert/core/util:flatbuffer_tools",
        "//litert/runtime:external_litert_buffer_context",
        "//litert/test:common",
        "//litert/test:simple_model_npu",
        "@org_tensorflow//tensorflow/lite:framework",
        "@org_tensorflow//tensorflow/lite/c:c_api_opaque",
        "@org_tensorflow//tensorflow/lite/c:common",
        "@org_tensorflow//tensorflow/lite/kernels:builtin_ops",
    ],
)
